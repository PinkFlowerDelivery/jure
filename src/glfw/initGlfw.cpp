#include "initGlfw.h"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan.h>

GLFWwindow* createWindow(int32_t width, int32_t height, const char* title) {

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    if (!glfwVulkanSupported()) {
        throw std::runtime_error("Vulkan unsupported");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    return window;
};
