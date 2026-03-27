#include "virtual_device_queue.h"
#include "queueUtils.h"
#include <set>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

VkDevice createDevice(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {

    std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    float queuePriority = 1.0f;
    std::set<uint32_t> uniqueQueueFamilies = {
        queueUtils::findQueueFamilies(physicalDevice, surface).graphicsFamily.value(),
        queueUtils::findQueueFamilies(physicalDevice, surface).presentFamily.value()};

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (const auto& queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
        graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = queueFamily;
        graphicsQueueCreateInfo.queueCount = 1;
        graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(graphicsQueueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

    VkDevice device;

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }

    return device;
};
