#pragma once
#include "vk/core/physical_device.h"
#include <GLFW/glfw3.h>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::window {

struct SwapchainDetails {
    VkFormat imageFormat;
    VkExtent2D extent;
    std::vector<VkImage> images;
};

std::pair<VkSwapchainKHR, SwapchainDetails>
createSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
                jure::vk::core::QueueFamilyContext queueFamilyContext, GLFWwindow* window);
} // namespace jure::vk::window
