#pragma once

#include <vulkan/vulkan_core.h>

struct VulkanContext {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
};

VulkanContext createContext();
