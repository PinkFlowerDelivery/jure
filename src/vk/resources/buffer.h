#pragma once

#include "fmt/base.h"
#include "vk/utils/memory_utils.h"
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::resources {

class Buffer {
    // We store the device here for cleanup in the destructor
    VkDevice device_;

    VkBuffer buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    size_t memorySize_ = 0;
    size_t size_ = 0;

    VkBuffer createBuffer(VkDevice device, VkBufferUsageFlags usageFlag,
                          VkSharingMode sharingMode) const;
    VkDeviceMemory allocateMemory(VkDevice device, uint32_t memoryTypeIndex);

  public:
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept
        : device_(other.device_), buffer_(other.buffer_), memory_(other.memory_),
          memorySize_(other.memorySize_), size_(other.size_) {

        other.device_ = VK_NULL_HANDLE;
        other.buffer_ = VK_NULL_HANDLE;
        other.memory_ = VK_NULL_HANDLE;
    };

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {

            cleanup();

            device_ = other.device_;
            buffer_ = other.buffer_;
            memory_ = other.memory_;
            memorySize_ = other.memorySize_;
            size_ = other.size_;

            other.device_ = VK_NULL_HANDLE;
            other.buffer_ = VK_NULL_HANDLE;
            other.memory_ = VK_NULL_HANDLE;
        }
        return *this;
    }

    void cleanup();

    ~Buffer();

    template <typename T>
    Buffer(VkPhysicalDevice pdevice, VkDevice device, const std::vector<T>& data,
           size_t bufferMemorySize, size_t bufferSize, VkBufferUsageFlags usageFlag,
           VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE)
        : device_(device) {

        if (bufferMemorySize == 0 || bufferSize == 0) {
            fmt::println("Buffer isn't created");
            return;
        }

        memorySize_ = bufferMemorySize;
        size_ = bufferSize;

        buffer_ = createBuffer(device, usageFlag, sharingMode);

        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(device, buffer_, &memReq);

        auto memoryTypeIndex = MemoryUtils::findMemoryType(pdevice, memReq.memoryTypeBits,
                                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        memory_ = allocateMemory(device, memoryTypeIndex);

        vkBindBufferMemory(device, buffer_, memory_, 0);

        void* memory = nullptr;

        if (vkMapMemory(device, memory_, 0, memorySize_, 0, &memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to map memory");
        }

        memcpy(memory, data.data(), memorySize_);

        vkUnmapMemory(device, memory_);
    }
    Buffer(VkPhysicalDevice pdevice, VkDevice device, size_t memorySize,
           VkBufferUsageFlags usageFlag, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

    template <typename T> void upload(VkDevice device, const std::vector<T>& data) {
        void* memory = nullptr;

        if (vkMapMemory(device, memory_, 0, memorySize_, 0, &memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to map memory");
        }

        memcpy(memory, data.data(), memorySize_);

        vkUnmapMemory(device, memory_);
    }

    void* mapMemory(size_t offset = 0);

    void unmapMemory();

    [[nodiscard]] VkBuffer getBuffer() const {
        return buffer_;
    }

    [[nodiscard]] size_t getMemorySize() const {
        return memorySize_;
    }
    [[nodiscard]] size_t getSize() const {
        return size_;
    }
};
} // namespace jure::vk::resources
