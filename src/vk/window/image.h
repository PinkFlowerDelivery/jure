#pragma once

#include "loaders/model_loader.h"
#include <vulkan/vulkan_core.h>

namespace jure::vk::window {

struct DepthImageContext {
    VkImage image;
    VkFormat format;
    VkDeviceMemory memory;
};

struct TextureImage {
    VkImage image;
    VkFormat format;
    VkDeviceMemory memory;
};

DepthImageContext createDepthImage(VkPhysicalDevice physicalDevice, VkDevice device,
                                   VkExtent2D extent);

std::vector<jure::vk::window::TextureImage>
createTextureImage(VkPhysicalDevice physicalDevice, VkDevice device,
                   std::vector<jure::loaders::Texture>& textures);
} // namespace jure::vk::window
