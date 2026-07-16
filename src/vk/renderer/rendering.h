#pragma once

#include "camera.h"
#include "vk/renderer/descriptorManager.h"
#include "vk/resources/index_buffer.h"
#include "vk/resources/uniform_buffer.h"
#include "vk/resources/vertex_buffer.h"
#include "vk/window/vk_window.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::renderer {

class Rendering {
    VkDevice device_;

    VkPipeline pipeline_;
    VkPipelineLayout pipelineLayout_;

    DescriptorManager dm_;

    std::vector<VkCommandBuffer> commandBuffers_;
    VkCommandPool commandPool_;
    VkQueue graphicQueue_;
    VkQueue presentQueue_;

    size_t currentFrame_ = 0;
    int32_t MAX_FRAMES_IN_FLIGHT_ = 3;

    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkFence> inFlightFences_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;

    std::vector<resources::UniformBuffer> uniformBuffers_;

    void createSyncObjects(VkDevice device, window::VulkanWindow& vkWindow);
    void recordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex,
                             resources::VertexBuffer& vBuffer, window::VulkanWindow& vkWindow,
                             resources::IndexBuffer& indexBuffer);

    VkPipelineLayout createPipelineLayout();
    void initUniformBuffers(VkPhysicalDevice pdevice, VkDevice device);

  public:
    Rendering(VkPhysicalDevice pdevice, VkDevice device, window::VulkanWindow& vkWindow,
              uint32_t graphicFamily, uint32_t presentFamilyr);

    ~Rendering();
    void drawFrame(VkDevice device, window::VulkanWindow& vkWindow,
                   resources::VertexBuffer& vBuffer, resources::IndexBuffer& indexBuffer,
                   ArcBallCamera camera);
};
} // namespace jure::vk::renderer
