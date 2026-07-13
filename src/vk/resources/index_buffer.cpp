#include "index_buffer.h"
#include "vk/resources/buffer_manager.h"
#include "vk/utils/memory_utils.h"
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

jure::vk::resources::IndexBuffer::IndexBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
                                              size_t bufferSize) {
    buffer_ = createBuffer(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferSize);

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, buffer_, &memReq);

    auto memoryTypeIndex = MemoryUtils::findMemoryType(physicalDevice, memReq.memoryTypeBits,
                                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    bufferMemory_ = allocateMemory(device, buffer_, memoryTypeIndex);

    vkBindBufferMemory(device, buffer_, bufferMemory_, 0);

    // We store the device here for cleanup in the destructor
    device_ = device;
};

jure::vk::resources::IndexBuffer::~IndexBuffer() {
    vkDestroyBuffer(device_, buffer_, nullptr);
    vkFreeMemory(device_, bufferMemory_, nullptr);
};

void jure::vk::resources::IndexBuffer::uploadIndices(VkDevice device,
                                                     const std::vector<uint32_t>& indicies) {

    void* memory = nullptr;
    size_ = indicies.size();

    VkDeviceSize bufferSize = sizeof(uint16_t) * indicies.size();

    if (vkMapMemory(device, bufferMemory_, 0, bufferSize, 0, &memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map memory");
    }

    memcpy(memory, indicies.data(), indicies.size() * sizeof(indicies[0]));

    vkUnmapMemory(device, bufferMemory_);
};
