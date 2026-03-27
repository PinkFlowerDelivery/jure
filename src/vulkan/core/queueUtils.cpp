#include "queueUtils.h"
#include "vulkan/context/vulkan_context.h"
#include <vector>

QueueFamilyContext queueUtils::findQueueFamilies(const VkPhysicalDevice& physicalDevice,
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
