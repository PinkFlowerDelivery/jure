#pragma once

#include "vk/core/physical_device.h"
#include <vulkan/vulkan_core.h>

namespace jure::vk::core {
VkDevice createVirtualDevice(VkPhysicalDevice physicalDevice,
                             jure::vk::core::QueueFamilyContext queueFamilyContext);
}
