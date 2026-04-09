#pragma once

#include <vulkan/vulkan_core.h>

struct DeviceResult {
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

DeviceResult createVirtualDevice(VkPhysicalDevice physicalDevice);
