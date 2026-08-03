#include "vk_window.h"
#include "vk/window/image.h"
#include "vk/window/image_views.h"
#include "vk/window/swapchain.h"
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

jvk::window::VulkanWindow::VulkanWindow(jure::vk::core::VulkanCore& core, GLFWwindow* window,
                                        std::vector<jure::loaders::Texture>& texture) {
    device_ = core.getDevice();

    auto [swapchain, swapchainDetails] =
        createSwapchain(core.getPhysicalDevice(), core.getDevice(), core.getSurface(),
                        core.getQueueFamilyContext(), window);

    swapchain_ = swapchain;
    swapchainDetails_ = swapchainDetails;

    depthImageContext_ =
        createDepthImage(core.getPhysicalDevice(), core.getDevice(), swapchainDetails_.extent);

    textureImages_ = createTextureImage(core.getPhysicalDevice(), core.getDevice(), texture);

    auto [imageViews, depthImageView, textureImageView] =
        createImageViews(core.getDevice(), swapchain_, depthImageContext_,
                         swapchainDetails_.imageFormat, textureImages_);

    textureImageViews_ = textureImageView;

    imageViews_ = imageViews;
    depthImageView_ = depthImageView;
}

jvk::window::VulkanWindow::~VulkanWindow() {
    vkDestroyImageView(device_, depthImageView_, nullptr);

    for (VkImageView textureImageView : textureImageViews_) {
        vkDestroyImageView(device_, textureImageView, nullptr);
    }

    for (VkImageView imageView : imageViews_) {
        vkDestroyImageView(device_, imageView, nullptr);
    }

    for (auto const& textureImage : textureImages_) {
        vkDestroyImage(device_, textureImage.image, nullptr);
        vkFreeMemory(device_, textureImage.memory, nullptr);
    }

    vkDestroyImage(device_, depthImageContext_.image, nullptr);
    vkFreeMemory(device_, depthImageContext_.memory, nullptr);

    vkDestroySwapchainKHR(device_, swapchain_, nullptr);
}
