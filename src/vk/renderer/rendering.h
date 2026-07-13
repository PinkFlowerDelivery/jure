#pragma once

#include "camera.h"
#include "vk/resources/index_buffer.h"
#include "vk/resources/uniform_buffer.h"
#include "vk/resources/vertex_buffer.h"
#include "vk/window/vk_window.h"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace jure::vk::renderer {

class Rendering {
    VkDevice device_;

    VkPipeline pipeline_;
    VkPipelineLayout pipelineLayout_;

    VkDescriptorSetLayout descriptorSetLayout_;
    VkDescriptorPool descriptorPool_;
    VkDescriptorSet descriptorSet_;

    VkCommandPool commandPool_;
    VkCommandBuffer commandBuffer_;
    VkQueue graphicQueue_;
    VkQueue presentQueue_;

    size_t currentFrame_ = 0;
    int32_t MAX_FRAMES_IN_FLIGHT_ = 3;

    VkFence inFlightFences_;
    VkSemaphore imageAvailableSemaphores_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;

    void createSyncObjects(VkDevice device);
    void recordCommandBuffer(uint32_t imageIndex, resources::VertexBuffer& vBuffer,
                             window::VulkanWindow& vkWindow, resources::IndexBuffer& indexBuffer);

    void createDescriptorPool();

  public:
    Rendering(VkDevice device, window::VulkanWindow& vkWindow, uint32_t graphicFamily,
              uint32_t presentFamily, resources::UniformBuffer& uniformBuffer);

    ~Rendering();
    void drawFrame(VkDevice device, window::VulkanWindow& vkWindow,
                   resources::VertexBuffer& vBuffer, resources::IndexBuffer& indexBuffer,
                   resources::UniformBuffer& uniformBuffer, ArcBallCamera camera);
};
} // namespace jure::vk::renderer
