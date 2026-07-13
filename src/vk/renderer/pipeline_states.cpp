#include "pipeline_states.h"
#include "vk/resources/vertex_buffer.h"
#include "vk/window/vk_window.h"
#include <fstream>
#include <ios>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>

std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

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

std::vector<VkPipelineShaderStageCreateInfo> createShaderStage(VkDevice device) {
    auto vertexShaderCode = readFile("shaders/vert.spv");
    auto fragmentShaderCode = readFile("shaders/frag.spv");

    VkShaderModule vertexShaderModule = createShaderModule(device, vertexShaderCode);
    VkShaderModule fragmentShaderModule = createShaderModule(device, fragmentShaderCode);

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

    return {vertexShaderStageCreateInfo, fragShaderStageCreateInfo};
};

VertexInputStateContainer createVertexInputState() {

    VertexInputStateContainer container;

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(jure::vk::resources::Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    container.bindingDescription = bindingDescription;

    // Position
    // WARN: Hardcoded format
    VkVertexInputAttributeDescription PosAttrDescription{};
    PosAttrDescription.location = 0;
    PosAttrDescription.binding = 0;
    PosAttrDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    PosAttrDescription.offset = offsetof(jure::vk::resources::Vertex, pos);

    // Color
    VkVertexInputAttributeDescription ColorAttrDescription{};
    ColorAttrDescription.location = 1;
    ColorAttrDescription.binding = 0;
    ColorAttrDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    ColorAttrDescription.offset = offsetof(jure::vk::resources::Vertex, color);

    container.attrDescriptions = {PosAttrDescription, ColorAttrDescription};

    VkPipelineVertexInputStateCreateInfo vertexStateCreateInfo{};
    vertexStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexStateCreateInfo.pVertexBindingDescriptions = &container.bindingDescription;
    vertexStateCreateInfo.vertexAttributeDescriptionCount = container.attrDescriptions.size();
    vertexStateCreateInfo.pVertexAttributeDescriptions = container.attrDescriptions.data();

    container.createInfo = vertexStateCreateInfo;

    return container;
};

VkPipelineInputAssemblyStateCreateInfo createInputAssemblyState() {
    VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo{};
    assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    return assemblyStateCreateInfo;
};

VkPipelineDynamicStateCreateInfo createDynamicState() {
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    return dynamicStateCreateInfo;
};

VkPipelineViewportStateCreateInfo createViewportState() {
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;

    return viewportStateCreateInfo;
};

VkPipelineRasterizationStateCreateInfo createRasterizationState() {
    VkPipelineRasterizationStateCreateInfo resterizationStateCreateInfo{};
    resterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    resterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    resterizationStateCreateInfo.lineWidth = 1.0f;
    resterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;

    resterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    return resterizationStateCreateInfo;
};

VkPipelineMultisampleStateCreateInfo createMultisampleState() {
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.minSampleShading = 1.0f;
    multisampleStateCreateInfo.pSampleMask = nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    return multisampleStateCreateInfo;
};

VkPipelineDepthStencilStateCreateInfo createDepthStencilState() {
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;

    return depthStencilStateCreateInfo;
};

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device) {
    VkDescriptorSetLayoutBinding uboBinding{};
    uboBinding.descriptorCount = 1;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.binding = 0;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &uboBinding;

    VkDescriptorSetLayout setLayout;
    if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &setLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set.");
    };

    return setLayout;
};

VkPipelineColorBlendStateCreateInfo
createColorBlendState(VkPipelineColorBlendAttachmentState colorBlendAttachment) {

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachment;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;

    return colorBlendStateCreateInfo;
};

VkPipelineRenderingCreateInfo createRendering(jure::vk::window::VulkanWindow& vkWindow) {

    VkPipelineRenderingCreateInfo renderingCreateInfo{};
    renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    renderingCreateInfo.colorAttachmentCount = 1;
    renderingCreateInfo.pColorAttachmentFormats = &vkWindow.getSwapchainDetails().imageFormat;
    renderingCreateInfo.depthAttachmentFormat = vkWindow.getDepthFormat();

    return renderingCreateInfo;
};
