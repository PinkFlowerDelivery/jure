#pragma once

#include <vulkan/vulkan_core.h>

namespace jure::vk::renderer {

class DescriptorManager {
    VkDevice device_;
    VkDescriptorSetLayout descriptorSetLayout_;
    VkDescriptorPool descriptorPool_;
    VkDescriptorSet descriptorSet_;

    void createDescriptorSetLayout();
    void createDescriptorPool();
    void allocateDescriptorSets();

  public:
    void init(VkDevice device);
    DescriptorManager() = default;
    ~DescriptorManager();

    DescriptorManager(const DescriptorManager&) = delete;
    DescriptorManager& operator=(const DescriptorManager&) = delete;

    void updateDescriptorSets(VkBuffer buffer, VkDeviceSize range);

    [[nodiscard]] VkDescriptorSetLayout& getDescriptorSetLayout() {
        return descriptorSetLayout_;
    }

    [[nodiscard]] VkDescriptorPool getDescriptorPool() const {
        return descriptorPool_;
    }

    [[nodiscard]] VkDescriptorSet& getDescriptorSet() {
        return descriptorSet_;
    }
};
} // namespace jure::vk::renderer
