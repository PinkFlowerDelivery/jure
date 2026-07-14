#include "pipeline_builder.h"
#include "vk/resources/vertex_buffer.h"
#include <fstream>
#include <glm/ext/scalar_constants.hpp>
#include <ios>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

PipelineBuilder::PipelineBuilder(VkDevice device) : device_(device) {};

PipelineBuilder::~PipelineBuilder() {
    for (const auto& module : shaderModules) {
        vkDestroyShaderModule(device_, module, nullptr);
    }
}

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule createShaderModule(VkDevice device, std::vector<char> shaderCode) {

    VkShaderModuleCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }

    return shaderModule;
}

PipelineBuilder& PipelineBuilder::useDefaultConfiguration() {
    // Shader stage
    auto vertexShaderCode = readFile("shaders/vert.spv");
    auto fragmentShaderCode = readFile("shaders/frag.spv");

    VkShaderModule vertexShaderModule = createShaderModule(device_, vertexShaderCode);
    VkShaderModule fragmentShaderModule = createShaderModule(device_, fragmentShaderCode);

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
    vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageCreateInfo.module = vertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
    fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageCreateInfo.module = fragmentShaderModule;
    fragShaderStageCreateInfo.pName = "main";
    fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    shaderStages.push_back(vertexShaderStageCreateInfo);
    shaderStages.push_back(fragShaderStageCreateInfo);

    shaderModules.push_back(vertexShaderModule);
    shaderModules.push_back(fragmentShaderModule);

    // Vertex input stage

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(jure::vk::resources::Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    bindingDescriptions.push_back(bindingDescription);

    // WARN: Hardcoded format
    VkVertexInputAttributeDescription PositionAttrDescription{};
    PositionAttrDescription.location = 0;
    PositionAttrDescription.binding = 0;
    PositionAttrDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    PositionAttrDescription.offset = offsetof(jure::vk::resources::Vertex, pos);

    VkVertexInputAttributeDescription ColorAttrDescription{};
    ColorAttrDescription.location = 1;
    ColorAttrDescription.binding = 0;
    ColorAttrDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    ColorAttrDescription.offset = offsetof(jure::vk::resources::Vertex, color);

    attrDescriptions.push_back(PositionAttrDescription);
    attrDescriptions.push_back(ColorAttrDescription);

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = attrDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attrDescriptions.data();

    // Assembly stage

    assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Dynamic states stage

    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateInfo.pDynamicStates = dynamicStates.data();

    // Viewport stage

    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.scissorCount = 1;

    // Rasterization stage

    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;

    rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    // Multisample stage

    multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleInfo.sampleShadingEnable = VK_FALSE;
    multisampleInfo.pSampleMask = nullptr;
    multisampleInfo.sampleShadingEnable = VK_FALSE;
    multisampleInfo.minSampleShading = 1.0f;
    multisampleInfo.pSampleMask = nullptr;
    multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleInfo.alphaToOneEnable = VK_FALSE;

    // Depth stencil stage

    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;
    depthStencilInfo.depthWriteEnable = VK_TRUE;
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;

    // Color blend stage

    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendAttachment;
    colorBlendInfo.logicOpEnable = VK_FALSE;

    // Dynamic rendering stage

    renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachmentFormats = &colorFormat_;
    renderingInfo.depthAttachmentFormat = depthFormat_;

    return *this;
};

PipelineBuilder& PipelineBuilder::setPipelineLayout(VkPipelineLayout layout) {
    pipelineLayout = layout;

    return *this;
}

PipelineBuilder& PipelineBuilder::setRenderingFormats(VkFormat colorFormat, VkFormat depthFormat) {
    colorFormat_ = colorFormat;
    depthFormat_ = depthFormat;

    return *this;
};

VkPipeline PipelineBuilder::build() {
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &renderingInfo;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.renderPass = nullptr;
    pipelineInfo.subpass = 0;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &assemblyInfo;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterizationInfo;
    pipelineInfo.pMultisampleState = &multisampleInfo;
    pipelineInfo.pDepthStencilState = &depthStencilInfo;
    pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.renderPass = nullptr;
    pipelineInfo.subpass = 0;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    return pipeline;
};
