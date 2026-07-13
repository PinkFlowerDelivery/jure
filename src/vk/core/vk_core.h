#pragma once

#include "vk/core/physical_device.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace jure::vk::core {

class VulkanCore {
    VkInstance instance_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    QueueFamilyContext queueFamilyContext_;
    VkDevice device_ = VK_NULL_HANDLE;

  public:
    VulkanCore(GLFWwindow* window);
    ~VulkanCore();

    [[nodiscard]] VkInstance getInstance() const {
        return instance_;
    }
    [[nodiscard]] VkSurfaceKHR getSurface() const {
        return surface_;
    }
    [[nodiscard]] VkPhysicalDevice& getPhysicalDevice() {
        return physicalDevice_;
    }
    [[nodiscard]] VkDevice getDevice() const {
        return device_;
    }
    [[nodiscard]] uint32_t getGraphicsFamilyIndex() const {
        return queueFamilyContext_.graphicsFamily.value();
    }
    [[nodiscard]] uint32_t getPresentFamilyIndex() const {
        return queueFamilyContext_.presentFamily.value();
    }
    [[nodiscard]] jure::vk::core::QueueFamilyContext& getQueueFamilyContext() {
        return queueFamilyContext_;
    }
};
} // namespace jure::vk::core
