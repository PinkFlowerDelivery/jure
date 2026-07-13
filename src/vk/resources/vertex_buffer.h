#pragma once

#include <cstddef>
#include <glm/ext/vector_float3.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::resources {

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
};

class VertexBuffer {

    // We store the device here for cleanup in the destructor
    VkDevice device_;

    VkBuffer buffer_;
    VkDeviceMemory bufferMemory_;
    size_t size_;

  public:
    void uploadVerticies(VkDevice device, const std::vector<Vertex>& vertices);
    VertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, size_t bufferSize);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    [[nodiscard]] VkBuffer getBuffer() const {
        return buffer_;
    }

    [[nodiscard]] size_t getSize() const {
        return size_;
    }
};

} // namespace jure::vk::resources
