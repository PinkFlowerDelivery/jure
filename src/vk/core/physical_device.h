#pragma once

#include <optional>
#include <vulkan/vulkan_core.h>

struct QueueFamilyContext {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isUnifiedQueue() const {
        return graphicsFamily.has_value() && presentFamily.has_value() &&
               graphicsFamily.value() == presentFamily.value();
    }
};

struct SelectedPhysicalDevice {
    VkPhysicalDevice device = VK_NULL_HANDLE;
    QueueFamilyContext queueFamilyContext;
};

SelectedPhysicalDevice pickPhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface);
