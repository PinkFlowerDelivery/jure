#include "image.h"
#include "vk/utils/memory_utils.h"
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                             const std::vector<VkFormat>& candidates,
                             VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if ((props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format");
}

jvk::window::DepthImageContext jvk::window::createDepthImage(VkPhysicalDevice physicalDevice,
                                                             VkDevice device,
                                                             const VkExtent2D extent) {
    jvk::window::DepthImageContext imageContext;

    imageContext.format = findSupportedFormat(
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.format = imageContext.format;
    createInfo.extent.width = extent.width;
    createInfo.extent.height = extent.height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(device, &createInfo, nullptr, &imageContext.image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, imageContext.image, &memRequirements);

    uint32_t memIndex = MemoryUtils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits,
                                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = memIndex;

    vkAllocateMemory(device, &allocInfo, nullptr, &imageContext.memory);
    vkBindImageMemory(device, imageContext.image, imageContext.memory, 0);

    return imageContext;
};
std::vector<jure::vk::window::TextureImage>
jvk::window::createTextureImage(VkPhysicalDevice physicalDevice, VkDevice device,
                                std::vector<jure::loaders::Texture>& textures) {
    std::vector<jure::vk::window::TextureImage> images;

    for (const auto& texture : textures) {
        TextureImage textureImage;
        textureImage.format =
            findSupportedFormat(physicalDevice, {VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_UNORM},
                                VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

        VkImageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType = VK_IMAGE_TYPE_2D;
        createInfo.format = textureImage.format;
        createInfo.extent = texture.extent;
        createInfo.mipLevels = 1;
        createInfo.arrayLayers = 1;
        createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (vkCreateImage(device, &createInfo, nullptr, &textureImage.image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture image.");
        };

        VkMemoryRequirements memReq;

        vkGetImageMemoryRequirements(device, textureImage.image, &memReq);

        uint32_t memIndex = MemoryUtils::findMemoryType(physicalDevice, memReq.memoryTypeBits,
                                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.memoryTypeIndex = memIndex;
        allocInfo.allocationSize = memReq.size;

        vkAllocateMemory(device, &allocInfo, nullptr, &textureImage.memory);
        vkBindImageMemory(device, textureImage.image, textureImage.memory, 0);

        images.push_back(textureImage);
    }

    return images;
};
