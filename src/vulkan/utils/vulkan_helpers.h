#pragma once

#include "vulkan/context/vulkan_context.h"
#include <cstdint>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace vulkan_helpers {
QueueFamilyContext findQueueFamilies(const VkPhysicalDevice& physicalDevice,
                                     const VkSurfaceKHR& surface);

VkSurfaceFormatKHR chooseFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkFormat findSupportedDepthFormat(const VkPhysicalDevice& physicalDevice,
                                  const std::vector<VkFormat>& candidates,
                                  VkFormatFeatureFlags features);

uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);

} // namespace vulkan_helpers
