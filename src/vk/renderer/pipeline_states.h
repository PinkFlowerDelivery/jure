#pragma once

#include "vk/window/vk_window.h"
#include <vector>
#include <vulkan/vulkan_core.h>

std::vector<VkPipelineShaderStageCreateInfo> createShaderStage(VkDevice device);

struct VertexInputStateContainer {
    VkVertexInputBindingDescription bindingDescription;
    std::vector<VkVertexInputAttributeDescription> attrDescriptions;
    VkPipelineVertexInputStateCreateInfo createInfo;
};

VertexInputStateContainer createVertexInputState();

VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState();

VkPipelineDynamicStateCreateInfo createDynamicState();

VkPipelineViewportStateCreateInfo createViewportState();

VkPipelineRasterizationStateCreateInfo createRasterizationState();

VkPipelineMultisampleStateCreateInfo createMultisampleState();

VkPipelineDepthStencilStateCreateInfo createDepthStencilState();

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device);

VkPipelineColorBlendStateCreateInfo
createColorBlendState(VkPipelineColorBlendAttachmentState colorBlendAttachment);

VkPipelineRenderingCreateInfo createRendering(jure::vk::window::VulkanWindow& vkWindow);
