#include "imageView.h"
#include "vulkan/utils/vulkan_helpers.h"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

ImageView createImageView(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
                          const VkSwapchainKHR& swapchain, const VkImage& depthImage,
                          const VkFormat& colorFormat) {
    ImageView view;

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);

    std::vector<VkImage> swapchainImages(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

    view.imageViews.resize(imageCount);

    for (size_t i = 0; i < imageCount; i++) {
        VkImage image = swapchainImages[i];

        VkImageViewCreateInfo createInfoColor{};
        createInfoColor.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfoColor.image = image;
        createInfoColor.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfoColor.format = colorFormat;

        createInfoColor.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfoColor.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfoColor.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfoColor.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfoColor.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfoColor.subresourceRange.baseMipLevel = 0;
        createInfoColor.subresourceRange.levelCount = 1;
        createInfoColor.subresourceRange.baseArrayLayer = 0;
        createInfoColor.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfoColor, nullptr, &view.imageViews[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view");
        }
    }

    VkImageViewCreateInfo createInfoDepth{};
    createInfoDepth.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfoDepth.image = depthImage;
    createInfoDepth.viewType = VK_IMAGE_VIEW_TYPE_2D;

    createInfoDepth.format = vulkan_helpers::findSupportedDepthFormat(
        physicalDevice,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    createInfoDepth.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfoDepth.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfoDepth.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfoDepth.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfoDepth.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    createInfoDepth.subresourceRange.baseMipLevel = 0;
    createInfoDepth.subresourceRange.levelCount = 1;
    createInfoDepth.subresourceRange.baseArrayLayer = 0;
    createInfoDepth.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfoDepth, nullptr, &view.depthImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create depth image view");
    }

    return view;
}
