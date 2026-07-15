
#include "descriptorManager.h"
#include "vk/resources/uniform_buffer.h"
#include <stdexcept>

namespace jvk = jure::vk;

jvk::renderer::DescriptorManager::~DescriptorManager() {
    vkDestroyDescriptorPool(device_, descriptorPool_, nullptr);
    vkDestroyDescriptorSetLayout(device_, descriptorSetLayout_, nullptr);
};

void jvk::renderer::DescriptorManager::init(VkDevice device) {
    device_ = device;
    createDescriptorSetLayout();
    createDescriptorPool();
    allocateDescriptorSets();
};

void jvk::renderer::DescriptorManager::createDescriptorPool() {
    VkDescriptorPoolSize uboPoolSize{};
    uboPoolSize.descriptorCount = 1;

    uboPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.pPoolSizes = &uboPoolSize;
    createInfo.poolSizeCount = 1;
    createInfo.maxSets = 1;

    VkDescriptorPool pool;

    if (vkCreateDescriptorPool(device_, &createInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool");
    }

    descriptorPool_ = pool;
};

void jvk::renderer::DescriptorManager::createDescriptorSetLayout() {
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
    if (vkCreateDescriptorSetLayout(device_, &createInfo, nullptr, &setLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set.");
    };

    descriptorSetLayout_ = setLayout;
};

void jvk::renderer::DescriptorManager::allocateDescriptorSets() {

    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = descriptorPool_;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &descriptorSetLayout_;

    VkDescriptorSet descriptorSet;

    if (vkAllocateDescriptorSets(device_, &allocateInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor set.");
    }

    descriptorSet_ = descriptorSet;
};

void jvk::renderer::DescriptorManager::updateDescriptorSets(
    std::vector<resources::UniformBuffer>& unifromBuffers, VkDeviceSize range) {

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
        writeDescriptorSet.dstSet = descriptorSet_;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device_, 1, &writeDescriptorSet, 0, nullptr);
    }
};
