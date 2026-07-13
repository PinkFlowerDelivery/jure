#include "virtual_device_queue.h"
#include <fmt/base.h>
#include <set>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

VkDevice
jure::vk::core::createVirtualDevice(VkPhysicalDevice physicalDevice,
                                    jure::vk::core::QueueFamilyContext queueFamilyContext) {

    std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                 "VK_KHR_dynamic_rendering"};
    float queuePriority = 1.0f;

    std::set<uint32_t> uniqueQueueFamilies = {queueFamilyContext.graphicsFamily.value(),
                                              queueFamilyContext.presentFamily.value()};

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for (const auto& queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
        graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueueCreateInfo.queueFamilyIndex = queueFamily;
        graphicsQueueCreateInfo.queueCount = 1;
        graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(graphicsQueueCreateInfo);
    }

    VkPhysicalDeviceVulkan13Features vk13Features{};
    vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vk13Features.synchronization2 = VK_TRUE;
    vk13Features.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &vk13Features;
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
