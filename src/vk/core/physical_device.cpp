#include "physical_device.h"
#include "fmt/base.h"
#include "vulkan/context/vulkan_context.h"
#include "vulkan/utils/vulkan_helpers.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

int32_t rateDevice(const VkPhysicalDevice& physicalDevice) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physicalDevice, &props);

    int32_t score = 0;

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        score += 100;
    }

    score += props.limits.maxImageDimension2D;
    score += props.limits.maxBoundDescriptorSets * 10;

    return score;
}

bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {
    VkPhysicalDeviceFeatures pdFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &pdFeatures);

    VkPhysicalDeviceProperties pdProps;
    vkGetPhysicalDeviceProperties(physicalDevice, &pdProps);

    QueueFamilyContext queueFamilyContext =
        vulkan_helpers::findQueueFamilies(physicalDevice, surface);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (!pdFeatures.geometryShader) {
        return false;
    }

    if (!queueFamilyContext.graphicsFamily.has_value() &&
        !queueFamilyContext.presentFamily.has_value()) {
        return false;
    }

    return true;
}

VkPhysicalDevice pickPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface) {
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int32_t bestScore = 0;
    for (const auto& device : physicalDevices) {
        if (!isDeviceSuitable(device, surface)) {
            continue;
        }

        int32_t score = rateDevice(device);

        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
        }
    }

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(bestDevice, &props);
    fmt::println("Prefer device: {}", props.deviceName);

    return bestDevice;
};
