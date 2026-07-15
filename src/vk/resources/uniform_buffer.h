#pragma once

#include "fmt/base.h"
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan_core.h>

namespace jure::vk::resources {

struct ArcBallCameraUniform {
    glm::mat4x4 view;
    glm::mat4x4 proj;
};

class UniformBuffer {
    // We store the device here for cleanup in the destructor
    VkDevice device_;

    VkBuffer buffer_;
    VkDeviceMemory bufferMemory_;
    size_t size_;

  public:
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer(VkPhysicalDevice physicalDevice, VkDevice device, size_t bufferSize);
    UniformBuffer(UniformBuffer&& other) noexcept
        : device_(other.device_), buffer_(other.buffer_), bufferMemory_(other.bufferMemory_),
          size_(other.size_) {

        other.device_ = VK_NULL_HANDLE;
        other.buffer_ = VK_NULL_HANDLE;
        other.bufferMemory_ = VK_NULL_HANDLE;
    };

    UniformBuffer& operator=(UniformBuffer&& other) noexcept {
        if (this != &other) {

            device_ = other.device_;
            buffer_ = other.buffer_;
            bufferMemory_ = other.bufferMemory_;

            other.device_ = VK_NULL_HANDLE;
            other.buffer_ = VK_NULL_HANDLE;
            other.bufferMemory_ = VK_NULL_HANDLE;
        }
        return *this;
    }
    ~UniformBuffer();
    void upload(VkDevice device, glm::mat4x4& view, glm::mat4x4& proj);

    [[nodiscard]] VkBuffer getBuffer() const {
        return buffer_;
    }

    [[nodiscard]] size_t getSize() const {
        return size_;
    }
};
} // namespace jure::vk::resources
