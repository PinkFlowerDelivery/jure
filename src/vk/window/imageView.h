#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct ImageView {
    std::vector<VkImageView> imageViews;
    VkImageView depthImageView;
};

ImageView createImageView(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
                          const VkSwapchainKHR& swapchain, const VkImage& depthImage,
                          const VkFormat& colorFormat);
