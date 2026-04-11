#include "swapchain.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }

    int32_t width;
    int32_t height;

    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

    actualExtent.width = std::clamp(actualExtent.width, capabilities.maxImageExtent.width,
                                    capabilities.minImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.maxImageExtent.height,
                                     capabilities.minImageExtent.height);

    return actualExtent;
}

VkFormat chooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat.format;
        }
    }
    return availableFormats[0].format;
}

SwapchainDetails createSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device,
                                 const VkSurfaceKHR surface,
                                 const QueueFamilyContext queueFamilyContext, GLFWwindow* window) {

    SwapchainDetails details{};

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormatArray(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                         surfaceFormatArray.data());

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

    details.extent = chooseSwapchainExtent(surfaceCapabilities, window);
    details.imageFormat = chooseSwapchainFormat(surfaceFormatArray);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
    createInfo.imageFormat = details.imageFormat;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageExtent = details.extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.clipped = VK_TRUE;

    std::vector<uint32_t> queueFamilyIndices;

    if (queueFamilyContext.isUnifiedQueue()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    } else {
        queueFamilyIndices.push_back(queueFamilyContext.graphicsFamily.value());
        queueFamilyIndices.push_back(queueFamilyContext.presentFamily.value());

        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &details.swapchain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    };
    return details;
}
