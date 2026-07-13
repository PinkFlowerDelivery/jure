#pragma once

#include <cstddef>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::resources {

class IndexBuffer {
    // We store the device here for cleanup in the destructor
    VkDevice device_;

    VkBuffer buffer_;
    VkDeviceMemory bufferMemory_;
    size_t size_;

  public:
    IndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, size_t bufferSize);
    ~IndexBuffer();
    void uploadIndices(VkDevice device, const std::vector<uint32_t>& indicies);

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&&) noexcept = default;
    IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

    [[nodiscard]] VkBuffer getBuffer() const {
        return buffer_;
    }

    [[nodiscard]] size_t getSize() const {
        return size_;
    }
};
} // namespace jure::vk::resources
