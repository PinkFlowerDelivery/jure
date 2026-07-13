#include "pipeline.h"
#include "vk/renderer/pipeline_states.h"
#include "vk/window/vk_window.h"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

std::tuple<VkPipeline, VkPipelineLayout, VkDescriptorSetLayout>
createGraphicsPipeline(VkDevice device, jure::vk::window::VulkanWindow& vkWindow) {
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    auto shaderStages = createShaderStage(device);

    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();

    VertexInputStateContainer vertexInputStateContainer = createVertexInputState();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateContainer.createInfo;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
        createInputAssemblyState();
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = createDynamicState();
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = createViewportState();
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
        createRasterizationState();
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = createMultisampleState();
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = createDepthStencilState();
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                               VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    colorBlendAttachmentState.blendEnable = VK_FALSE;

    auto* setLayout = createDescriptorSetLayout(device);

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
        createColorBlendState(colorBlendAttachmentState);

    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;

    VkPipelineRenderingCreateInfo renderingCreateInfo = createRendering(vkWindow);
    pipelineCreateInfo.pNext = &renderingCreateInfo;

    pipelineCreateInfo.pTessellationState = nullptr;
    pipelineCreateInfo.renderPass = nullptr;
    pipelineCreateInfo.subpass = 0;

    // pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pSetLayouts = &setLayout;
    pipelineLayoutCreateInfo.setLayoutCount = 1;

    VkPipelineLayout pipelineLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    pipelineCreateInfo.layout = pipelineLayout;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                  &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    return {pipeline, pipelineLayout, setLayout};
};
