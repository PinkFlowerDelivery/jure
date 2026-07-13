#pragma once

#include <vulkan/vulkan_core.h>

namespace jure::vk::window {

struct DepthImageContext {
    VkImage image;
    VkFormat format;
    VkDeviceMemory memory;
};

DepthImageContext createImage(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent);
} // namespace jure::vk::window
