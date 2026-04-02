#pragma once

#include "vulkan/core/imageView.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <vulkan/vulkan_core.h>

struct QueueFamilyContext {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isUnifiedQueue() const {
        return graphicsFamily.has_value() && presentFamily.has_value() &&
               graphicsFamily.value() == presentFamily.value();
    }
};

struct VulkanContext {

    QueueFamilyContext queueFamilyContext;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    ImageView imageViews;
};

VulkanContext createContext(GLFWwindow* window);
void destroyContext(VulkanContext& ctx, GLFWwindow* window);
