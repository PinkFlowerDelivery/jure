#include "vk_window.h"
#include "vk/window/image.h"
#include "vk/window/image_views.h"
#include "vk/window/swapchain.h"
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

jvk::window::VulkanWindow::VulkanWindow(jvk::core::VulkanCore& core, GLFWwindow* window) {
    device_ = core.getDevice();

    auto [swapchain, swapchainDetails] =
        createSwapchain(core.getPhysicalDevice(), core.getDevice(), core.getSurface(),
                        core.getQueueFamilyContext(), window);

    swapchain_ = swapchain;
    swapchainDetails_ = swapchainDetails;

    depthImageContext_ =
        createImage(core.getPhysicalDevice(), core.getDevice(), swapchainDetails_.extent);

    auto [imageViews, depthImageView] = createImageViews(
        core.getDevice(), swapchain_, depthImageContext_, swapchainDetails_.imageFormat);

    imageViews_ = imageViews;
    depthImageView_ = depthImageView;
}

jvk::window::VulkanWindow::~VulkanWindow() {
    vkDestroyImageView(device_, depthImageView_, nullptr);

    for (VkImageView imageView : imageViews_) {
        vkDestroyImageView(device_, imageView, nullptr);
    }

    vkDestroyImage(device_, depthImageContext_.image, nullptr);
    vkFreeMemory(device_, depthImageContext_.memory, nullptr);

    vkDestroySwapchainKHR(device_, swapchain_, nullptr);
}
