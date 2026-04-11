#pragma once

#include "vk/core/vk_core.h"
#include "vk/window/image.h"
#include "vk/window/imageViews.h"
#include "vk/window/swapchain.h"
#include <vulkan/vulkan_core.h>
class VulkanWindow {

    // Need for destroy swapchain and depth image
    VkDevice device_;

    SwapchainDetails swapchain_;
    DepthImage depthImage_{};
    ImageView imageViews_{};

  public:
    VulkanWindow(VulkanCore& core, GLFWwindow* window);
    ~VulkanWindow();
};
