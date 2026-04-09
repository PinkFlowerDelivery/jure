#pragma once

#include <vulkan/vulkan_core.h>

VkPhysicalDevice pickPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface);
