#include "physical_device.h"
#include "fmt/base.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

uint32_t findQueueFamilies(const VkPhysicalDevice& physicalDevice) {
    uint32_t queueFamiliesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);

    std::vector<VkQueueFamilyProperties> familyProps(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount,
                                             familyProps.data());

    uint32_t i = 0;
    for (auto& props : familyProps) {
        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
        i++;
    }
}

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

bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice) {
    VkPhysicalDeviceFeatures pdFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &pdFeatures);

    VkPhysicalDeviceProperties pdProps;
    vkGetPhysicalDeviceProperties(physicalDevice, &pdProps);

    if (!pdFeatures.geometryShader) {
        return false;
    }

    return true;
}

VkPhysicalDevice pickPhysicalDevice(const VkInstance& instance) {
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int32_t bestScore = 0;
    for (const auto& device : physicalDevices) {
        if (!isDeviceSuitable(device)) {
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
