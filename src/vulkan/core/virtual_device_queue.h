#pragma once

#include <vulkan/vulkan_core.h>

VkDevice createDevice(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
