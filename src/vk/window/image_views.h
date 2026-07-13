#pragma once

#include "vk/window/image.h"
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace jure::vk::window {

struct ImageView {};

std::pair<std::vector<VkImageView>, VkImageView>
createImageViews(VkDevice device, VkSwapchainKHR swapchain,
                 jure::vk::window::DepthImageContext depthImageContext, VkFormat colorFormat);
} // namespace jure::vk::window
