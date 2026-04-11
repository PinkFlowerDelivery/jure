#pragma once

#include "vk/window/image.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct ImageView {
    std::vector<VkImageView> imageViews;
    VkImageView depthImageView;
};

ImageView createImageViews(VkDevice device, VkSwapchainKHR swapchain, DepthImage depthImage,
                           VkFormat colorFormat);
