#pragma once

#include <GLFW/glfw3.h>
#include <optional>
#include <vulkan/vulkan_core.h>

struct QueueFamilyContext {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
};

struct VulkanContext {

    QueueFamilyContext queueFamilyContext;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
};

VulkanContext createContext(GLFWwindow* window);
void destroyContext(VulkanContext& ctx, GLFWwindow* window);
