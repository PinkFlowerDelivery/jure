#pragma once

#include <vulkan/vulkan_core.h>

struct DepthImage {
    VkImage image;
    VkFormat format;
    VkDeviceMemory memory;
};

DepthImage createImage(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent);
