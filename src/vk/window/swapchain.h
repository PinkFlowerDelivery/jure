#pragma once
#include "vk/core/physical_device.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

struct SwapchainDetails {
    VkSwapchainKHR swapchain;
    VkFormat imageFormat;
    VkExtent2D extent;
};

SwapchainDetails createSwapchain(VkPhysicalDevice physicalDevice, VkDevice device,
                                 VkSurfaceKHR surface, QueueFamilyContext queueFamilyContext,
                                 GLFWwindow* window);
