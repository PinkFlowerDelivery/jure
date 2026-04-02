#include "image.h"
#include "vulkan/utils/vulkan_helpers.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

DepthImage createImage(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
                       const VkExtent2D& extent) {
    DepthImage image;

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = vulkan_helpers::findSupportedDepthFormat(
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    createInfo.extent.width = extent.width;
    createInfo.extent.height = extent.height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(device, &createInfo, nullptr, &image.image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image.image, &memRequirements);

    uint32_t memIndex = vulkan_helpers::findMemoryType(
        physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memIndex;

    vkAllocateMemory(device, &allocInfo, nullptr, &image.memory);
    vkBindImageMemory(device, image.image, image.memory, 0);

    return image;
};
