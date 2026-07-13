#include "buffer_manager.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

VkBuffer jure::vk::resources::createBuffer(VkDevice device, VkBufferUsageFlags usageFlag,
                                           size_t bufferSize, VkSharingMode sharingMode) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlag;
    bufferInfo.sharingMode = sharingMode;
    VkBuffer buffer;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer.");
    }

    return buffer;
}

VkDeviceMemory jure::vk::resources::allocateMemory(VkDevice device, VkBuffer buffer,
                                                   uint32_t memoryTypeIndex) {
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory bufferMemory;
    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer memory!");
    };

    return bufferMemory;
}
