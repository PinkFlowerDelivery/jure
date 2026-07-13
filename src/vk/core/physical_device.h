#pragma once

#include <optional>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace jure::vk::core {

struct QueueFamilyContext {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isUnifiedQueue() const {
        return graphicsFamily.has_value() && presentFamily.has_value() &&
               graphicsFamily.value() == presentFamily.value();
    }
};

std::pair<VkPhysicalDevice, QueueFamilyContext> pickPhysicalDevice(const VkInstance& instance,
                                                                   const VkSurfaceKHR& surface);
} // namespace jure::vk::core
