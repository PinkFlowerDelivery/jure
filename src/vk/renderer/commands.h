#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamilyIndex);
std::vector<VkCommandBuffer> createCommandBuffer(VkDevice device, VkCommandPool commandPool);
