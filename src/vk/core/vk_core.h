#pragma once

#include "vk/core/physical_device.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

class VulkanCore {
    VkInstance instance_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    SelectedPhysicalDevice physicalDevice_{};
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
    [[nodiscard]] VkDevice getDevice() const {
        return device_;
    }
    [[nodiscard]] const SelectedPhysicalDevice& getPhysicalDevice() const {
        return physicalDevice_;
    }
};
