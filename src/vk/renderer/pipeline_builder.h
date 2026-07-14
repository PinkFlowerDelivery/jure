#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

class PipelineBuilder {
    VkDevice device_;

  public:
    std::vector<VkShaderModule> shaderModules;
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    VkPipelineViewportStateCreateInfo viewportInfo{};
    VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    VkPipelineRenderingCreateInfo renderingInfo{};
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attrDescriptions;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};

  private:
    VkFormat colorFormat_ = VK_FORMAT_UNDEFINED;
    VkFormat depthFormat_ = VK_FORMAT_UNDEFINED;

  public:
    PipelineBuilder(VkDevice device);
    ~PipelineBuilder();
    PipelineBuilder& setPipelineLayout(VkPipelineLayout pipelineLayout);
    PipelineBuilder& setRenderingFormats(VkFormat colorFormat, VkFormat depthFormat);
    PipelineBuilder& useDefaultConfiguration();
    VkPipeline build();
};
