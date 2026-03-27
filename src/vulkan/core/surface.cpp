#include "surface.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window) {

    VkSurfaceKHR surface;

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create surface.");
    }

    return surface;
};
