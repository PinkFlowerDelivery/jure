#pragma once

#include "camera.h"
#include "vk/renderer/descriptor_manager.h"
#include "vk/resources/buffer.h"
#include "vk/window/vk_window.h"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jure::vk::renderer {

class Rendering {
    VkDevice device_;
    uint32_t imagesCount_;

    VkPipeline pipeline_;
    VkPipelineLayout pipelineLayout_;

    DescriptorManager dm_;

    std::vector<VkCommandBuffer> commandBuffers_;
    VkCommandPool commandPool_;
    VkQueue graphicQueue_;
    VkQueue presentQueue_;

    size_t currentFrame_ = 0;
    uint32_t MAX_FRAMES_IN_FLIGHT_ = 3;

    std::vector<VkSemaphore> imageAvailableSemaphores_;
    std::vector<VkFence> inFlightFences_;
    std::vector<VkSemaphore> renderFinishedSemaphores_;

    std::vector<resources::Buffer> uniformBuffers_;

    void createSyncObjects(VkDevice device, window::VulkanWindow& vkWindow);
    void recordCommandBuffer(VkCommandBuffer currentCommandBuffer, uint32_t imageIndex,
                             resources::Buffer& vBuffer, window::VulkanWindow& vkWindow,
                             resources::Buffer& indexBuffer,
                             std::vector<loaders::Texture>& textures,
                             jure::vk::resources::Buffer& stagingBuffer);

    VkPipelineLayout createPipelineLayout();
    void initUniformBuffers(VkPhysicalDevice pdevice, VkDevice device);

  public:
    Rendering(VkPhysicalDevice pdevice, VkDevice device, window::VulkanWindow& vkWindow,
              uint32_t graphicFamily, uint32_t presentFamily);

    ~Rendering();
    void drawFrame(VkDevice device, window::VulkanWindow& vkWindow, resources::Buffer& vBuffer,
                   resources::Buffer& indexBuffer, ArcBallCamera camera,
                   std::vector<loaders::Texture>& textures,
                   jure::vk::resources::Buffer& stagingBuffer);
};
} // namespace jure::vk::renderer
