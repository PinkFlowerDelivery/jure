#pragma once
#include "vulkan/context/vulkan_context.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

struct SwapchainDetails {
    VkSwapchainKHR swapchain;
    VkFormat imageFormat;
    VkExtent2D extent;
};

SwapchainDetails createSwapchain(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                                 const VkSurfaceKHR& surface,
                                 const QueueFamilyContext& queueFamilyContext, GLFWwindow* window);
