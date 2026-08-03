#pragma once

#include "vk/window/image.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace jure::vk::window {

std::tuple<std::vector<VkImageView>, VkImageView, std::vector<VkImageView>>
createImageViews(VkDevice device, VkSwapchainKHR swapchain,
                 jure::vk::window::DepthImageContext depthImageContext, VkFormat colorFormat,
                 std::vector<jure::vk::window::TextureImage> textureImages);
} // namespace jure::vk::window
