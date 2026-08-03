#pragma once

#include "loaders/model_loader.h"
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
    std::vector<TextureImage> textureImages_;
    std::vector<VkImageView> textureImageViews_;

  public:
    VulkanWindow(jure::vk::core::VulkanCore& core, GLFWwindow* window,
                 std::vector<jure::loaders::Texture>& texture);
    ~VulkanWindow();

    [[nodiscard]] VkSwapchainKHR& getSwapchain() {
        return swapchain_;
    }
    [[nodiscard]] SwapchainDetails& getSwapchainDetails() {
        return swapchainDetails_;
    }
    [[nodiscard]] VkExtent2D getExtent() const {
        return swapchainDetails_.extent;
    }
    [[nodiscard]] uint32_t getExtentWidth() const {
        return swapchainDetails_.extent.width;
    }
    [[nodiscard]] uint32_t getExtentHeight() const {
        return swapchainDetails_.extent.height;
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
    [[nodiscard]] std::vector<TextureImage>& getTextureImages() {
        return textureImages_;
    }
    [[nodiscard]] std::vector<VkImageView>& getTextureImageView() {
        return textureImageViews_;
    }
};

} // namespace jure::vk::window
