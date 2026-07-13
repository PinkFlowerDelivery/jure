#pragma once

#include <vulkan/vulkan_core.h>

VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamilyIndex);
VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool commandPool);
