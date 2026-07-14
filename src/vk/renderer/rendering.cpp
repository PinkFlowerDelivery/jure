#include "rendering.h"
#include "camera.h"
#include "vk/renderer/commands.h"
#include "vk/renderer/descriptorManager.h"
#include "vk/renderer/memory_barriers.h"
#include "vk/renderer/pipeline_builder.h"
#include "vk/resources/uniform_buffer.h"
#include "vk/resources/vertex_buffer.h"
#include "vk/window/swapchain.h"
#include "vk/window/vk_window.h"
#include <array>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

VkPipelineLayout jvk::renderer::Rendering::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pSetLayouts = &dm_.getDescriptorSetLayout();
    pipelineLayoutInfo.setLayoutCount = 1;

    VkPipelineLayout layout;

    if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    return layout;
};

jvk::renderer::Rendering::Rendering(VkDevice device, jvk::window::VulkanWindow& vkWindow,
                                    uint32_t graphicFamily, uint32_t presentFamily,
                                    resources::UniformBuffer& uniformBuffer)
    : device_(device) {

    dm_.init(device_);
    dm_.updateDescriptorSets(uniformBuffer.getBuffer(), sizeof(resources::ArcBallCameraUniform));

    vkGetDeviceQueue(device, graphicFamily, 0, &graphicQueue_);

    vkGetDeviceQueue(device, presentFamily, 0, &presentQueue_);

    pipelineLayout_ = createPipelineLayout();

    pipeline_ = PipelineBuilder(device)
                    .setPipelineLayout(pipelineLayout_)
                    .setRenderingFormats(vkWindow.getImageFormat(), vkWindow.getDepthFormat())
                    .useDefaultConfiguration()
                    .build();

    commandPool_ = createCommandPool(device, presentFamily);
    commandBuffer_ = createCommandBuffer(device, commandPool_);
    createSyncObjects(device);
}

jvk::renderer::Rendering::~Rendering() {
    vkDeviceWaitIdle(device_);
    vkDestroyFence(device_, inFlightFences_, nullptr);
    vkDestroySemaphore(device_, imageAvailableSemaphores_, nullptr);

    for (int32_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; i++) {
        vkDestroySemaphore(device_, renderFinishedSemaphores_[i], nullptr);
    }

    vkDestroyCommandPool(device_, commandPool_, nullptr);
    vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
    vkDestroyPipeline(device_, pipeline_, nullptr);
};

void jvk::renderer::Rendering::drawFrame(VkDevice device, jvk::window::VulkanWindow& vkWindow,
                                         jvk::resources::VertexBuffer& vBuffer,
                                         jvk::resources::IndexBuffer& indexBuffer,
                                         resources::UniformBuffer& uniformBuffer,
                                         ArcBallCamera camera) {

    vkWaitForFences(device, 1, &inFlightFences_, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFences_);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, vkWindow.getSwapchain(), UINT64_MAX, imageAvailableSemaphores_,
                          VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffer_, 0);

    recordCommandBuffer(imageIndex, vBuffer, vkWindow, indexBuffer);

    auto viewMatrix = camera.getViewMatrix();
    auto projMatrix = camera.getProjectionMatrix(vkWindow.getSwapchainDetails().extent.width,
                                                 vkWindow.getSwapchainDetails().extent.height);

    uniformBuffer.upload(device_, viewMatrix, projMatrix);

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores_;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores_[imageIndex];
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;

    if (vkQueueSubmit(graphicQueue_, 1, &submitInfo, inFlightFences_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit");
    };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores_[imageIndex];

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkWindow.getSwapchain();

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue_, &presentInfo);

    currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT_;
}

void jvk::renderer::Rendering::recordCommandBuffer(uint32_t imageIndex,
                                                   jvk::resources::VertexBuffer& vBuffer,
                                                   jvk::window::VulkanWindow& vkWindow,
                                                   jvk::resources::IndexBuffer& indexBuffer) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkImage currentImage = vkWindow.getImages()[imageIndex];

    auto colorBarrier = createColorBarrier(currentImage);
    auto depthBarrier = createDepthBarrier(vkWindow.getDepthImage());
    auto presentBarrier = createPresentBarrier(currentImage);

    VkDependencyInfo colorDepInfo{};
    colorDepInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    colorDepInfo.imageMemoryBarrierCount = 1;
    colorDepInfo.pImageMemoryBarriers = &colorBarrier;

    VkDependencyInfo depthDepInfo{};
    depthDepInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depthDepInfo.imageMemoryBarrierCount = 1;
    depthDepInfo.pImageMemoryBarriers = &depthBarrier;

    VkDependencyInfo presentDepInfo{};
    presentDepInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    presentDepInfo.imageMemoryBarrierCount = 1;
    presentDepInfo.pImageMemoryBarriers = &presentBarrier;

    VkImageView currentImageView = vkWindow.getImageViews()[imageIndex];

    VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachmentInfo.imageView = currentImageView;
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
    depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    depthAttachmentInfo.imageView = vkWindow.getDepthImageView();
    depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    depthAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachmentInfo.clearValue.depthStencil = {1.0f, 0};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = {{0, 0}, vkWindow.getSwapchainDetails().extent};
    renderingInfo.layerCount = 1;
    renderingInfo.viewMask = 0;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachmentInfo;
    renderingInfo.pDepthAttachment = &depthAttachmentInfo;
    renderingInfo.pStencilAttachment = nullptr;

    vkBeginCommandBuffer(commandBuffer_, &beginInfo);
    std::array<VkDeviceSize, 1> offsets = {0};
    VkBuffer buffer = vBuffer.getBuffer();

    vkCmdBindVertexBuffers(commandBuffer_, 0, 1, &buffer, offsets.data());
    vkCmdBindDescriptorSets(commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1,
                            &dm_.getDescriptorSet(), 0, nullptr);
    vkCmdBindIndexBuffer(commandBuffer_, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindPipeline(commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

    vkCmdPipelineBarrier2(commandBuffer_, &colorDepInfo);
    vkCmdPipelineBarrier2(commandBuffer_, &depthDepInfo);

    vkCmdBeginRendering(commandBuffer_, &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = vkWindow.getSwapchainDetails().extent.width;
    viewport.height = vkWindow.getSwapchainDetails().extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer_, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vkWindow.getSwapchainDetails().extent;
    vkCmdSetScissor(commandBuffer_, 0, 1, &scissor);

    if (indexBuffer.getSize() > 0) {
        vkCmdDrawIndexed(commandBuffer_, indexBuffer.getSize(), 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer_, vBuffer.getSize(), 1, 0, 0);
    }

    vkCmdEndRendering(commandBuffer_);
    vkCmdPipelineBarrier2(commandBuffer_, &presentDepInfo);
    vkEndCommandBuffer(commandBuffer_);
};

void jvk::renderer::Rendering::createSyncObjects(VkDevice device) {
    renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT_);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int32_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; i++) {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores_) !=
                VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                              &renderFinishedSemaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sync objects");
        }
    }
};
