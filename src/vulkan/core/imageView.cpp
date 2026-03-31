#include "imageView.h"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

std::pair<VkImageView, std::vector<VkImageView>> createImageView(const VkDevice& device,
                                                                 const VkSwapchainKHR& swapchain) {

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);

    std::vector<VkImage> swapchainImages(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

    std::vector<VkImageView> imageViews(imageCount);

    for (size_t i = 0; i < imageCount; i++) {
        VkImage image = swapchainImages[i];

        VkImageViewCreateInfo createInfoColor{};
        createInfoColor.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfoColor.image = image;
        createInfoColor.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfoColor.format = VK_FORMAT_B8G8R8A8_SRGB;

        createInfoColor.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfoColor.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfoColor.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfoColor.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfoColor.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfoColor.subresourceRange.baseMipLevel = 0;
        createInfoColor.subresourceRange.levelCount = 1;
        createInfoColor.subresourceRange.baseArrayLayer = 0;
        createInfoColor.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfoColor, nullptr, &imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view");
        }
    }

    VkImageView imageView;

    VkImageViewCreateInfo createInfoDepth{};
    createInfoDepth.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfoDepth.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfoDepth.format = VK_FORMAT_D32_SFLOAT;

    createInfoDepth.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    createInfoDepth.subresourceRange.baseMipLevel = 0;
    createInfoDepth.subresourceRange.levelCount = 1;
    createInfoDepth.subresourceRange.baseArrayLayer = 0;
    createInfoDepth.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfoDepth, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create depth image view");
    }

    return {imageView, imageViews};
}
