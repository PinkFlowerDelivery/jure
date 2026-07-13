#include "camera.h"
#include "vk/resources/uniform_buffer.h"
#include <vector>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glfw/initGlfw.h"
#include "tiny_gltf.h"
#include "vk/core/vk_core.h"
#include "vk/renderer/rendering.h"
#include "vk/resources/index_buffer.h"
#include "vk/resources/vertex_buffer.h"
#include "vk/window/vk_window.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace jure::vk;

int main() {
    // <-
    std::vector<resources::Vertex> vertices = {
        {{-0.75f, -0.75f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // V0 LEFT TOP
        {{-0.75f, 0.25f, 0.0f}, {0.0f, 1.0f, 0.0f}},  // V1 LEFT BOTTOM
        {{0.75f, 0.25f, 0.0f}, {0.0f, 0.0f, 1.0f}},   // V2 RIGHT BOTTOM
        {{0.75f, -0.75f, 0.0f}, {1.0f, 0.0f, 0.0f}},  // V3 RIGHT TOP
    };

    std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0, 0, 2, 1};

    GLFWwindow* window = createWindow(640, 480, "vk");

    core::VulkanCore core(window);
    window::VulkanWindow vkWindow(core, window);

    size_t vertexBufferSize = vertices.size() * sizeof(vertices[0]);
    resources::VertexBuffer vertexBuffer(core.getPhysicalDevice(), core.getDevice(),
                                         vertexBufferSize);
    vertexBuffer.uploadVerticies(core.getDevice(), vertices);

    size_t indexBufferSize = indices.size() * sizeof(indices[0]);
    resources::IndexBuffer indexBuffer(core.getPhysicalDevice(), core.getDevice(), indexBufferSize);
    indexBuffer.uploadIndices(core.getDevice(), indices);

    resources::UniformBuffer uniformBuffer(core.getPhysicalDevice(), core.getDevice(),
                                           sizeof(resources::ArcBallCameraUniform));

    renderer::Rendering rendering(core.getDevice(), vkWindow, core.getGraphicsFamilyIndex(),
                                  core.getPresentFamilyIndex(), uniformBuffer);

    ArcBallCamera camera{};

    double oldX = 0;
    double oldY = 0;
    double posX = 0;
    double posY = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwGetCursorPos(window, &posX, &posY);

            auto deltaX = oldX - posX;
            auto deltaY = oldY - posY;

            camera.addYaw(deltaX);
            camera.addPitch(deltaY);

            oldX = posX;
            oldY = posY;
        }
        rendering.drawFrame(core.getDevice(), vkWindow, vertexBuffer, indexBuffer, uniformBuffer,
                            camera);
    }
}
