#include "vulkan_helpers.h"

#include "vulkan/context/vulkan_context.h"
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

QueueFamilyContext vulkan_helpers::findQueueFamilies(const VkPhysicalDevice& physicalDevice,
                                                     const VkSurfaceKHR& surface) {
    uint32_t queueFamiliesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);

    std::vector<VkQueueFamilyProperties> familyProps(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount,
                                             familyProps.data());

    QueueFamilyContext context;

    uint32_t i = 0;
    for (auto& props : familyProps) {

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport) {
            context.graphicsFamily = i;
        }

        if (presentSupport) {
            context.presentFamily = i;
        }
    }

    return context;
}

VkSurfaceFormatKHR
vulkan_helpers::chooseFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& format : availableFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return availableFormats[0];
};

VkFormat vulkan_helpers::findSupportedDepthFormat(const VkPhysicalDevice& physicalDevice,
                                                  const std::vector<VkFormat>& candidates,
                                                  VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;

        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        if ((props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
};

uint32_t vulkan_helpers::findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter,
                                        VkMemoryPropertyFlags properties) {

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (size_t i = 0; i < memProperties.memoryTypeCount; i++) {

        bool isCompatible = (typeFilter & (1 << i));

        bool hasProps = (memProperties.memoryTypes[i].propertyFlags & properties) == properties;

        if (isCompatible && hasProps) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
};
