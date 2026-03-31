#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

std::pair<VkImageView, std::vector<VkImageView>> createImageView(const VkDevice& device,
                                                                 const VkSwapchainKHR& swapchain);
