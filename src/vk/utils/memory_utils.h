#pragma once

#include <stdexcept>
#include <vulkan/vulkan_core.h>
namespace MemoryUtils {

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter,
                        VkMemoryPropertyFlags properties);
} // namespace MemoryUtils
