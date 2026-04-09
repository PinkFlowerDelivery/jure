#pragma once

#include <vulkan/vulkan_core.h>

struct DepthImage {
    VkImage image;
    VkDeviceMemory memory;
};

DepthImage createImage(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
                       const VkExtent2D& extent);
