#pragma once

#include "vk/resources/buffer.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::renderer {

class DescriptorManager {
    VkDevice device_;
    std::vector<VkDescriptorSetLayout> descriptorSetLayout_;
    VkDescriptorPool descriptorPool_;
    VkSampler sampler_;

    std::vector<VkDescriptorSet> descriptorSet_;

  public:
    uint32_t createDescriptorSetLayout(uint32_t descriptorCount, uint32_t binding,
                                       VkDescriptorType descriptorType,
                                       VkShaderStageFlags shaderStageFlag);
    void createDescriptorPool(std::vector<VkDescriptorPoolSize>& descriptorPoolSizes);
    void allocateDescriptorSets();

    DescriptorManager(VkDevice device) : device_(device) {};
    ~DescriptorManager();

    DescriptorManager(const DescriptorManager&) = delete;
    DescriptorManager& operator=(const DescriptorManager&) = delete;

    void updateDescriptorSets(std::vector<resources::Buffer>& unifromBuffers, VkDeviceSize range,
                              std::vector<VkImageView>& textureImageView);

    [[nodiscard]] VkDescriptorSetLayout* getDescriptorSetLayout() {
        return descriptorSetLayout_.data();
    }
    [[nodiscard]] size_t getDescriptorSetLayoutSize() const {
        return descriptorSetLayout_.size();
    }

    [[nodiscard]] VkDescriptorPool getDescriptorPool() const {
        return descriptorPool_;
    }

    [[nodiscard]] VkDescriptorSet* getDescriptorSet() {
        return descriptorSet_.data();
    }
    [[nodiscard]] size_t getDescriptorSetSize() const {
        return descriptorSet_.size();
    }
};
} // namespace jure::vk::renderer
