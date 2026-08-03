#include "buffer.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

jvk::resources::Buffer::~Buffer() {
    if (memory_ != VK_NULL_HANDLE) {
        vkFreeMemory(device_, memory_, nullptr);
    }

    if (buffer_ != VK_NULL_HANDLE) {
        vkDestroyBuffer(device_, buffer_, nullptr);
    }
};

void jvk::resources::Buffer::cleanup() {
    if (memory_ != VK_NULL_HANDLE) {
        vkFreeMemory(device_, memory_, nullptr);
    }

    if (buffer_ != VK_NULL_HANDLE) {
        vkDestroyBuffer(device_, buffer_, nullptr);
    }
};

jvk::resources::Buffer::Buffer(VkPhysicalDevice pdevice, VkDevice device, size_t memorySize,
                               VkBufferUsageFlags usageFlag, VkSharingMode sharingMode)
    : device_(device) {
    memorySize_ = memorySize;

    buffer_ = createBuffer(device, usageFlag, sharingMode);

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, buffer_, &memReq);

    auto memoryTypeIndex = MemoryUtils::findMemoryType(pdevice, memReq.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    memory_ = allocateMemory(device, memoryTypeIndex);

    vkBindBufferMemory(device, buffer_, memory_, 0);
}

void* jvk::resources::Buffer::mapMemory(size_t offset) {
    void* memory = nullptr;

    if (vkMapMemory(device_, memory_, offset, memorySize_, 0, &memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map memory");
    };

    return memory;
}
void jvk::resources::Buffer::unmapMemory() {
    vkUnmapMemory(device_, memory_);
};

VkBuffer jvk::resources::Buffer::createBuffer(VkDevice device, VkBufferUsageFlags usageFlag,
                                              VkSharingMode sharingMode) const {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = memorySize_;
    bufferInfo.usage = usageFlag;
    bufferInfo.sharingMode = sharingMode;

    VkBuffer buffer;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer.");
    }

    return buffer;
}
VkDeviceMemory jvk::resources::Buffer::allocateMemory(VkDevice device, uint32_t memoryTypeIndex) {
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer_, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    VkDeviceMemory bufferMemory;
    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate memory!");
    };

    return bufferMemory;
}
