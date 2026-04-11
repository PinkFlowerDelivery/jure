#pragma once

#include "vk/core/physical_device.h"
#include <vulkan/vulkan_core.h>

VkDevice createVirtualDevice(VkPhysicalDevice physicalDevice,
                             QueueFamilyContext queueFamilyContext);
