#include "camera.h"
#include "fmt/base.h"
#include "glfw/initGlfw.h"
#include "loaders/gltf_loader.h"
#include "loaders/model_loader.h"
#include "loaders/obj_loader.h"
#include "tiny_gltf.h"
#include "vk/core/vk_core.h"
#include "vk/renderer/rendering.h"
#include "vk/resources/buffer.h"
#include "vk/window/vk_window.h"
#include <GLFW/glfw3.h>
#include <cstring>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

using namespace jure::vk;

static double scrollY = 0.0;
void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) {
    scrollY = yoffset;
}

std::string getFileExtension(const std::string& filepath) {
    auto dotPosition = filepath.rfind('.');
    if (dotPosition == std::string::npos) {
        throw std::runtime_error("Unexpected file extension");
    }
    std::string fileExt = filepath.substr(dotPosition);

    return fileExt;
}

jure::loaders::Texture dummyTexture = {{1, 1, 1}, {255, 255, 255, 255}};

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        fmt::println("Use: jure [filepath]");
        return 0;
    }

    std::string filepath = argv[1];
    auto fileExt = getFileExtension(filepath);

    std::unique_ptr<jure::loaders::ModelLoader> modelLoader = nullptr;

    if (fileExt == ".gltf") {
        modelLoader = std::make_unique<jure::loaders::GLTFLoader>(true);
    } else if (fileExt == ".glb") {
        modelLoader = std::make_unique<jure::loaders::GLTFLoader>(false);
    } else if (fileExt == ".obj") {
        modelLoader = std::make_unique<jure::loaders::OBJLoader>();
    } else {
        throw std::runtime_error("Unsupported model format.");
    }

    jure::loaders::Model model = modelLoader->load(filepath);
    fmt::println("V: {}; I: {}", model.vertices.size(), model.indices.size());

    if (model.textures.size() <= 16) {
        size_t needPush = 16 - model.textures.size();
        for (size_t i = 0; i < needPush; i++) {
            model.textures.push_back(dummyTexture);
        }
    }

    GLFWwindow* window = createWindow(640, 480, "vk");

    core::VulkanCore core(window);
    window::VulkanWindow vkWindow(core, window, model.textures);

    size_t verticesSize = sizeof(model.vertices[0]) * model.vertices.size();
    jure::vk::resources::Buffer vertexBuffer(core.getPhysicalDevice(), core.getDevice(),
                                             model.vertices, verticesSize, model.vertices.size(),
                                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    size_t indicesSize = sizeof(model.indices[0]) * model.indices.size();
    jure::vk::resources::Buffer indexBuffer(core.getPhysicalDevice(), core.getDevice(),
                                            model.indices, indicesSize, model.indices.size(),
                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    size_t stagingSize = 0;
    for (auto& texture : model.textures) {
        stagingSize += sizeof(texture.data[0]) * texture.data.size();
    };

    resources::Buffer stagingBuffer = resources::Buffer(
        core.getPhysicalDevice(), core.getDevice(), stagingSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    void* stagingMemory = stagingBuffer.mapMemory();

    size_t offset = 0;
    for (const auto& texture : model.textures) {
        std::memcpy((static_cast<uint8_t*>(stagingMemory) + offset), texture.data.data(),
                    texture.data.size());

        offset += texture.data.size();
    }

    stagingBuffer.unmapMemory();

    renderer::Rendering rendering(core.getPhysicalDevice(), core.getDevice(), vkWindow,
                                  core.getGraphicsFamilyIndex(), core.getPresentFamilyIndex());

    ArcBallCamera camera{};

    double oldX = 0;
    double oldY = 0;
    double posX = 0;
    double posY = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glfwSetScrollCallback(window, scroll_callback);
        camera.setDistance(scrollY);
        scrollY = 0.0;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwGetCursorPos(window, &posX, &posY);

            auto deltaX = oldX - posX;
            auto deltaY = oldY - posY;

            camera.addYaw(deltaX);
            camera.addPitch(deltaY);

            oldX = posX;
            oldY = posY;
        }
        rendering.drawFrame(core.getDevice(), vkWindow, vertexBuffer, indexBuffer, camera,
                            model.textures, stagingBuffer);
    }
}
