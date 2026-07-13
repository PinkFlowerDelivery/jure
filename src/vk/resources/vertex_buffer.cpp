#include "vertex_buffer.h"
#include "vk/resources/buffer_manager.h"
#include "vk/utils/memory_utils.h"
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

jure::vk::resources::VertexBuffer::VertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                                                size_t bufferSize) {
    buffer_ =
        jure::vk::resources::createBuffer(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferSize);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer_, &memRequirements);

    auto memoryTypeIndex = MemoryUtils::findMemoryType(
        physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    bufferMemory_ = jure::vk::resources::allocateMemory(device, buffer_, memoryTypeIndex);

    vkBindBufferMemory(device, buffer_, bufferMemory_, 0);

    // We store the device here for cleanup in the destructor
    device_ = device;
}

void jure::vk::resources::VertexBuffer::uploadVerticies(VkDevice device,
                                                        const std::vector<Vertex>& vertices) {

    void* memory = nullptr;
    size_ = vertices.size();

    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    if (vkMapMemory(device, bufferMemory_, 0, bufferSize, 0, &memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map memory");
    }

    memcpy(memory, vertices.data(), vertices.size() * sizeof(vertices[0]));

    vkUnmapMemory(device, bufferMemory_);
};

jure::vk::resources::VertexBuffer::~VertexBuffer() {
    vkDestroyBuffer(device_, buffer_, nullptr);
    vkFreeMemory(device_, bufferMemory_, nullptr);
}
