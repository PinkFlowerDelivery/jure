#pragma once

#include "vulkan/context/vulkan_context.h"
#include <vulkan/vulkan.h>

namespace queueUtils {
QueueFamilyContext findQueueFamilies(const VkPhysicalDevice& physicalDevice,
                                     const VkSurfaceKHR& surface);
} // namespace queueUtils
