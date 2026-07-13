#pragma once

#include <vulkan/vulkan_core.h>

namespace jure::vk::resources {
VkBuffer createBuffer(VkDevice device, VkBufferUsageFlags usageFlag, size_t bufferSize,
                      VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);
VkDeviceMemory allocateMemory(VkDevice device, VkBuffer buffer, uint32_t memoryTypeIndex);
} // namespace jure::vk::resources
