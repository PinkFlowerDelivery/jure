#include "camera.h"
#include "fmt/base.h"
#include "glfw/initGlfw.h"
#include "loaders/gltf_loader.h"
#include "loaders/model_loader.h"
#include "loaders/obj_loader.h"
#include "tiny_gltf.h"
#include "vk/core/vk_core.h"
#include "vk/renderer/rendering.h"
#include "vk/resources/index_buffer.h"
#include "vk/resources/vertex_buffer.h"
#include "vk/window/vk_window.h"
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <stdexcept>
#include <string>

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
    } else if (fileExt == ".stl") {
        // modelLoader = std::make_unique<jure::loaders::STLLoader>();
    } else {
        throw std::runtime_error("Unsupported model format.");
    }

    auto [vertices, indices] = modelLoader->load(filepath);
    fmt::println("V: {}; I: {}", vertices.size(), indices.size());

    GLFWwindow* window = createWindow(640, 480, "vk");

    core::VulkanCore core(window);
    window::VulkanWindow vkWindow(core, window);

    size_t vertexBufferSize = vertices.size() * sizeof(vertices[0]);
    resources::VertexBuffer vertexBuffer(core.getPhysicalDevice(), core.getDevice(),
                                         vertexBufferSize);
    vertexBuffer.uploadVerticies(core.getDevice(), vertices);

    size_t indexBufferSize = indices.size() * sizeof(indices[0]);

    resources::IndexBuffer indexBuffer(core.getPhysicalDevice(), core.getDevice(), indexBufferSize);
    if (indexBufferSize != 0) {
        indexBuffer.uploadIndices(core.getDevice(), indices);
    }
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
        rendering.drawFrame(core.getDevice(), vkWindow, vertexBuffer, indexBuffer, camera);
    }
}
