#include "descriptor_manager.h"
#include <alloca.h>
#include <cstdint>
#include <stdexcept>
#include <sys/types.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

jvk::renderer::DescriptorManager::~DescriptorManager() {
    vkDestroySampler(device_, sampler_, nullptr);
    vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);

    for (auto& layout : descriptorSetLayout_) {
        vkDestroyDescriptorSetLayout(device_, layout, nullptr);
    }
};

uint32_t jvk::renderer::DescriptorManager::createDescriptorSetLayout(
    uint32_t descriptorCount, uint32_t binding, VkDescriptorType descriptorType,
    VkShaderStageFlags shaderStageFlag) {

    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.descriptorCount = descriptorCount;
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.stageFlags = shaderStageFlag;

    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &layoutBinding;

    VkDescriptorSetLayout setLayout;
    if (vkCreateDescriptorSetLayout(device_, &createInfo, nullptr, &setLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set.");
    };

    descriptorSetLayout_.push_back(setLayout);

    return descriptorSetLayout_.size() - 1;
};

void jvk::renderer::DescriptorManager::createDescriptorPool(
    std::vector<VkDescriptorPoolSize>& descriptorPoolSizes) {

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pPoolSizes = descriptorPoolSizes.data();
    createInfo.poolSizeCount = descriptorPoolSizes.size();
    createInfo.maxSets = 2;

    if (vkCreateDescriptorPool(device_, &createInfo, nullptr, &descriptorPool_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }
};

void jvk::renderer::DescriptorManager::allocateDescriptorSets() {

    for (auto& layout : descriptorSetLayout_) {
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = descriptorPool_;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &layout;

        VkDescriptorSet descriptorSet;

        if (vkAllocateDescriptorSets(device_, &allocateInfo, &descriptorSet) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor set.");
        }

        descriptorSet_.push_back(descriptorSet);
    }
};

void jvk::renderer::DescriptorManager::updateDescriptorSets(
    std::vector<resources::Buffer>& unifromBuffers, VkDeviceSize range,
    std::vector<VkImageView>& textureImageView) {

    VkSamplerCreateInfo samplerInfo{};

    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 1.0f;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    if (vkCreateSampler(device_, &samplerInfo, nullptr, &sampler_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create sampler");
    };

    std::vector<VkDescriptorImageInfo> descriptorImageView(textureImageView.size());
    for (size_t i = 0; i < textureImageView.size(); i++) {
        descriptorImageView[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        descriptorImageView[i].imageView = textureImageView[i];
        descriptorImageView[i].sampler = sampler_;
    }

    VkWriteDescriptorSet imageWriteDescriptorSet{};
    imageWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    imageWriteDescriptorSet.descriptorCount = textureImageView.size();
    imageWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageWriteDescriptorSet.dstBinding = 0;
    imageWriteDescriptorSet.dstSet = descriptorSet_[1];
    imageWriteDescriptorSet.dstArrayElement = 0;
    imageWriteDescriptorSet.pImageInfo = descriptorImageView.data();

    vkUpdateDescriptorSets(device_, 1, &imageWriteDescriptorSet, 0, nullptr);

    for (const auto& buffer : unifromBuffers) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer.getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = range;

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstSet = descriptorSet_[0];
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device_, 1, &writeDescriptorSet, 0, nullptr);
    }
};
