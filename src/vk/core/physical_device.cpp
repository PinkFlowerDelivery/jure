#include "physical_device.h"
#include "fmt/base.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <vulkan/vulkan_core.h>

int32_t rateDevice(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physicalDevice, &props);

    int32_t score = 0;

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 100;
    }

    if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        score += 10;
    }

    score += props.limits.maxImageDimension2D;
    score += props.limits.maxBoundDescriptorSets * 10;

    return score;
}

bool isDeviceSuitable(VkPhysicalDevice physicalDevice,
                      jure::vk::core::QueueFamilyContext queueFamilyContext) {

    VkPhysicalDeviceFeatures pdFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &pdFeatures);

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensionProps;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                         extensionProps.data());

    bool isExtSupported = false;
    for (const auto& extension : extensionProps) {
        if (std::string_view(extension.extensionName) == VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) {
            isExtSupported = true;
            break;
        }
    }

    if (isExtSupported) {
        return false;
    }

    if (!pdFeatures.geometryShader) {
        return false;
    }

    if (!queueFamilyContext.graphicsFamily.has_value() &&
        !queueFamilyContext.presentFamily.has_value()) {
        return false;
    }

    return true;
}

jure::vk::core::QueueFamilyContext findQueueFamilies(VkPhysicalDevice physicalDevice,
                                                     VkSurfaceKHR surface) {
    uint32_t queueFamiliesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);

    std::vector<VkQueueFamilyProperties> familyProps(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount,
                                             familyProps.data());

    jure::vk::core::QueueFamilyContext context;

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

std::pair<VkPhysicalDevice, jure::vk::core::QueueFamilyContext>
jure::vk::core::pickPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface) {

    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
    jure::vk::core::QueueFamilyContext bestQueueFamilies;
    int32_t bestScore = 0;

    for (const auto& device : physicalDevices) {
        auto families = findQueueFamilies(device, surface);

        if (!isDeviceSuitable(device, families)) {
            continue;
        }

        int32_t score = rateDevice(device);

        if (score > bestScore) {
            bestScore = score;
            bestDevice = device;
            bestQueueFamilies = families;
        }
    }

    VkPhysicalDevice physicalDevice = bestDevice;
    QueueFamilyContext queueFamilyContext = bestQueueFamilies;

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    fmt::println("Queue families: \n  Graphic: {}\n  Present: {}\n  Is unified: {}",
                 bestQueueFamilies.graphicsFamily.has_value(),
                 bestQueueFamilies.presentFamily.has_value(), bestQueueFamilies.isUnifiedQueue());

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    fmt::println("Selected gpu: {}", deviceProperties.deviceName);

    return {physicalDevice, queueFamilyContext};
};
