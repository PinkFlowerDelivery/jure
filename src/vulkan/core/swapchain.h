#pragma once
#include "vulkan/context/vulkan_context.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

VkSwapchainKHR createSwapchain(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                               const VkSurfaceKHR& surface,
                               const QueueFamilyContext& queueFamilyContext, GLFWwindow* window);
