#include "physical_device.h"
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

QueueFamilyContext findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
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

bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface,
                      QueueFamilyContext queueFamilyContext) {
    VkPhysicalDeviceFeatures pdFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &pdFeatures);

    VkPhysicalDeviceProperties pdProps;
    vkGetPhysicalDeviceProperties(physicalDevice, &pdProps);

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

SelectedPhysicalDevice pickPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface) {

    SelectedPhysicalDevice result{};

    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    int32_t bestScore = 0;

    QueueFamilyContext queueFamilyContext;

    for (const auto& device : physicalDevices) {
        queueFamilyContext = findQueueFamilies(device, surface);

        if (!isDeviceSuitable(device, surface, queueFamilyContext)) {
            continue;
        }

        int32_t score = rateDevice(device);

        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
        }
    }

    result.device = bestDevice;
    result.queueFamilyContext = queueFamilyContext;

    return result;
};
