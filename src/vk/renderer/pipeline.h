#pragma once

#include "vk/window/vk_window.h"
#include <vulkan/vulkan_core.h>

std::tuple<VkPipeline, VkPipelineLayout, VkDescriptorSetLayout>
createGraphicsPipeline(VkDevice device, jure::vk::window::VulkanWindow& vkWindow);
