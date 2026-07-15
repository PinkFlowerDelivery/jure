#include "uniform_buffer.h"
#include "vk/resources/buffer_manager.h"
#include "vk/utils/memory_utils.h"
#include <cstring>
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

jure::vk::resources::UniformBuffer::UniformBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                                                  size_t bufferSize) {
    // We store the device here for cleanup in the destructor
    device_ = device;
    buffer_ = jvk::resources::createBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer_, &memRequirements);

    auto memoryTypeIndex = MemoryUtils::findMemoryType(
        physicalDevice, memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    bufferMemory_ = jvk::resources::allocateMemory(device, buffer_, memoryTypeIndex);

    vkBindBufferMemory(device, buffer_, bufferMemory_, 0);
}

void jure::vk::resources::UniformBuffer::upload(VkDevice device, glm::mat4x4& view,
                                                glm::mat4x4& proj) {

    void* memory = nullptr;
    size_ = sizeof(view) + sizeof(proj);

    ArcBallCameraUniform cameraData = {view, proj};

    if (vkMapMemory(device, bufferMemory_, 0, size_, 0, &memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map memory");
    }

    memcpy(memory, &cameraData, sizeof(ArcBallCameraUniform));

    vkUnmapMemory(device, bufferMemory_);
};

jure::vk::resources::UniformBuffer::~UniformBuffer() {
    if (buffer_ != VK_NULL_HANDLE) {
        vkDestroyBuffer(device_, buffer_, nullptr);
    }
    if (bufferMemory_ != VK_NULL_HANDLE) {
        vkFreeMemory(device_, bufferMemory_, nullptr);
    }
}
