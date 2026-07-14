#pragma once

#include "vk/core/vk_core.h"
#include "vk/window/image.h"
#include "vk/window/swapchain.h"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::window {

class VulkanWindow {

    // Need for destroy swapchain and depth image
    VkDevice device_;

    VkSwapchainKHR swapchain_;
    SwapchainDetails swapchainDetails_;
    DepthImageContext depthImageContext_{};
    std::vector<VkImageView> imageViews_;
    VkImageView depthImageView_;

  public:
    [[nodiscard]] VkSwapchainKHR& getSwapchain() {
        return swapchain_;
    }
    [[nodiscard]] SwapchainDetails& getSwapchainDetails() {
        return swapchainDetails_;
    }
    [[nodiscard]] std::vector<VkImage>& getImages() {
        return swapchainDetails_.images;
    }
    [[nodiscard]] VkImage getDepthImage() const {
        return depthImageContext_.image;
    }
    [[nodiscard]] std::vector<VkImageView>& getImageViews() {
        return imageViews_;
    }
    [[nodiscard]] VkImageView getDepthImageView() const {
        return depthImageView_;
    }
    [[nodiscard]] VkFormat getDepthFormat() const {
        return depthImageContext_.format;
    }
    [[nodiscard]] VkFormat getImageFormat() const {
        return swapchainDetails_.imageFormat;
    }
    VulkanWindow(jure::vk::core::VulkanCore& core, GLFWwindow* window);
    ~VulkanWindow();
};

} // namespace jure::vk::window
