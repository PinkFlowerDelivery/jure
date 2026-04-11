#include "vk_window.h"
#include "vk/window/image.h"
#include "vk/window/imageViews.h"
#include "vk/window/swapchain.h"
#include <vulkan/vulkan_core.h>

VulkanWindow::VulkanWindow(VulkanCore& core, GLFWwindow* window) {
    device_ = core.getDevice();

    swapchain_ =
        createSwapchain(core.getPhysicalDevice().device, core.getDevice(), core.getSurface(),
                        core.getPhysicalDevice().queueFamilyContext, window);

    depthImage_ = createImage(core.getPhysicalDevice().device, core.getDevice(), swapchain_.extent);
    imageViews_ = createImageViews(core.getDevice(), swapchain_.swapchain, depthImage_,
                                   swapchain_.imageFormat);
}

VulkanWindow::~VulkanWindow() {
    vkDestroyImageView(device_, imageViews_.depthImageView, nullptr);

    for (VkImageView imageView : imageViews_.imageViews) {
        vkDestroyImageView(device_, imageView, nullptr);
    }

    vkDestroyImage(device_, depthImage_.image, nullptr);
    vkFreeMemory(device_, depthImage_.memory, nullptr);

    vkDestroySwapchainKHR(device_, swapchain_.swapchain, nullptr);
}
