#include "vk_core.h"
#include "instance.h"
#include "physical_device.h"
#include "surface.h"
#include "virtual_device_queue.h"
#include <fmt/core.h>
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

jure::vk::core::VulkanCore::VulkanCore(GLFWwindow* window) {
    instance_ = jvk::core::createInstance();
    surface_ = jvk::core::createSurface(instance_, window);

    auto [physicalDevice, queueContext] = jure::vk::core::pickPhysicalDevice(instance_, surface_);

    physicalDevice_ = physicalDevice;
    queueFamilyContext_ = queueContext;

    device_ = jure::vk::core::createVirtualDevice(physicalDevice_, queueFamilyContext_);
}

jure::vk::core::VulkanCore::~VulkanCore() {
    vkDestroyDevice(device_, nullptr);
    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    vkDestroyInstance(instance_, nullptr);
};
