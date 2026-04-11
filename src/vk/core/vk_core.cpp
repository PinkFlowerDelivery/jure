#include "vk_core.h"
#include "instance.h"
#include "physical_device.h"
#include "surface.h"
#include "virtual_device_queue.h"
#include <fmt/core.h>
#include <vulkan/vulkan_core.h>

VulkanCore::VulkanCore(GLFWwindow* window) {
    instance_ = createInstance();
    surface_ = createSurface(instance_, window);
    physicalDevice_ = pickPhysicalDevice(instance_, surface_);
    device_ = createVirtualDevice(physicalDevice_.device, physicalDevice_.queueFamilyContext);

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice_.device, &deviceProperties);
    fmt::println("Selected gpu: {}", deviceProperties.deviceName);
}

VulkanCore::~VulkanCore() {
    vkDestroyDevice(device_, nullptr);
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyInstance(instance_, nullptr);
};
