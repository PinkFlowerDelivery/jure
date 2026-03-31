#pragma once

#include "vulkan/context/vulkan_context.h"
#include <vulkan/vulkan_core.h>

VkDevice createDevice(const VkPhysicalDevice& physicalDevice,
                      const QueueFamilyContext& queueFamilyContext);
