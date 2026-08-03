#include "rendering.h"
#include "camera.h"
#include "vk/renderer/commands.h"
#include "vk/renderer/descriptor_manager.h"
#include "vk/renderer/memory_barriers.h"
#include "vk/renderer/pipeline_builder.h"
#include "vk/resources/buffer.h"
#include "vk/window/vk_window.h"
#include <array>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace jvk = jure::vk;

VkPipelineLayout jvk::renderer::Rendering::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pSetLayouts = dm_.getDescriptorSetLayout();
    pipelineLayoutInfo.setLayoutCount = dm_.getDescriptorSetLayoutSize();

    VkPipelineLayout layout;

    if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    return layout;
};

void jvk::renderer::Rendering::initUniformBuffers(VkPhysicalDevice pdevice, VkDevice device) {
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; i++) {
        resources::Buffer ubo(pdevice, device, sizeof(resources::ArcBallCameraUniform),
                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
        uniformBuffers_.push_back(std::move(ubo));
    }
};

jvk::renderer::Rendering::Rendering(VkPhysicalDevice pdevice, VkDevice device,
                                    jvk::window::VulkanWindow& vkWindow, uint32_t graphicFamily,
                                    uint32_t presentFamily)
    : device_(device), imagesCount_(vkWindow.getImages().size()), dm_(device) {

    initUniformBuffers(pdevice, device);

    createSyncObjects(device, vkWindow);

    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16},
    };

    dm_.createDescriptorSetLayout(1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                  VK_SHADER_STAGE_VERTEX_BIT);
    dm_.createDescriptorSetLayout(vkWindow.getTextureImageView().size(), 0,
                                  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                  VK_SHADER_STAGE_FRAGMENT_BIT);
    dm_.createDescriptorPool(poolSizes);
    dm_.allocateDescriptorSets();

    dm_.updateDescriptorSets(uniformBuffers_, sizeof(resources::ArcBallCameraUniform),
                             vkWindow.getTextureImageView());

    vkGetDeviceQueue(device, graphicFamily, 0, &graphicQueue_);

    vkGetDeviceQueue(device, presentFamily, 0, &presentQueue_);

    pipelineLayout_ = createPipelineLayout();

    pipeline_ = PipelineBuilder(device)
                    .setPipelineLayout(pipelineLayout_)
                    .setRenderingFormats(vkWindow.getImageFormat(), vkWindow.getDepthFormat())
                    .useDefaultConfiguration()
                    .build();

    commandPool_ = createCommandPool(device, presentFamily);
    commandBuffers_ = createCommandBuffer(device, commandPool_);
}

jvk::renderer::Rendering::~Rendering() {
    vkDeviceWaitIdle(device_);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; i++) {
        vkDestroyFence(device_, inFlightFences_[i], nullptr);
        vkDestroySemaphore(device_, imageAvailableSemaphores_[i], nullptr);
    }

    for (uint32_t i = 0; i < imagesCount_; i++) {
        vkDestroySemaphore(device_, renderFinishedSemaphores_[i], nullptr);
    }

    vkDestroyCommandPool(device_, commandPool_, nullptr);
    vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
    vkDestroyPipeline(device_, pipeline_, nullptr);
};

void jvk::renderer::Rendering::drawFrame(VkDevice device, jvk::window::VulkanWindow& vkWindow,
                                         jvk::resources::Buffer& vBuffer,
                                         jvk::resources::Buffer& indexBuffer, ArcBallCamera camera,
                                         std::vector<loaders::Texture>& textures,
                                         jvk::resources::Buffer& stagingBuffer) {
    vkWaitForFences(device_, 1, &inFlightFences_[currentFrame_], VK_TRUE, UINT64_MAX);
    vkResetFences(device_, 1, &inFlightFences_[currentFrame_]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, vkWindow.getSwapchain(), UINT64_MAX,
                          imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffers_[currentFrame_], 0);

    recordCommandBuffer(commandBuffers_[currentFrame_], imageIndex, vBuffer, vkWindow, indexBuffer,
                        textures, stagingBuffer);

    auto& currentUniformBuffer = uniformBuffers_[currentFrame_];

    glm::mat4x4 viewMatrix = camera.getViewMatrix();
    uint32_t width = vkWindow.getExtentWidth();
    uint32_t height = vkWindow.getExtentHeight();
    glm::mat4x4 projMatrix = camera.getProjectionMatrix(width, height);

    void* mappedMemory = currentUniformBuffer.mapMemory();

    memcpy(mappedMemory, &viewMatrix, sizeof(viewMatrix));
    memcpy(static_cast<uint8_t*>(mappedMemory) + sizeof(viewMatrix), &projMatrix,
           sizeof(viewMatrix));

    currentUniformBuffer.unmapMemory();

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT_;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores_[currentFrame_];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores_[imageIndex];
    submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];

    if (vkQueueSubmit(graphicQueue_, 1, &submitInfo, inFlightFences_[currentFrame_]) !=
        VK_SUCCESS) {
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

void jvk::renderer::Rendering::recordCommandBuffer(
    VkCommandBuffer currentCommandBuffer, uint32_t imageIndex, jvk::resources::Buffer& vBuffer,
    jvk::window::VulkanWindow& vkWindow, jvk::resources::Buffer& indexBuffer,
    std::vector<loaders::Texture>& textures, jvk::resources::Buffer& stagingBuffer) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkImage currentImage = vkWindow.getImages()[imageIndex];

    auto colorBarrier = createColorBarrier(currentImage);
    auto depthBarrier = createDepthBarrier(vkWindow.getDepthImage());
    auto presentBarrier = createPresentBarrier(currentImage);

    std::vector<VkImageMemoryBarrier2> textureWriteBarriers;
    std::vector<VkImageMemoryBarrier2> textureShaderReadBarriers;

    for (size_t i = 0; i < textures.size(); i++) {
        VkImageMemoryBarrier2 writeBarrier =
            createTextureWriteBarrier(vkWindow.getTextureImages()[i].image);
        textureWriteBarriers.push_back(writeBarrier);

        VkImageMemoryBarrier2 shaderReadBarrier =
            createTextureShaderReadBarrier(vkWindow.getTextureImages()[i].image);
        textureShaderReadBarriers.push_back(shaderReadBarrier);
    }

    std::vector<VkImageMemoryBarrier2> barriers{colorBarrier, depthBarrier};

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.imageMemoryBarrierCount = barriers.size();
    depInfo.pImageMemoryBarriers = barriers.data();

    VkDependencyInfo presentDepInfo{};
    presentDepInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    presentDepInfo.imageMemoryBarrierCount = 1;
    presentDepInfo.pImageMemoryBarriers = &presentBarrier;

    VkDependencyInfo textureWriteDepInfo{};
    textureWriteDepInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    textureWriteDepInfo.imageMemoryBarrierCount = textureWriteBarriers.size();
    textureWriteDepInfo.pImageMemoryBarriers = textureWriteBarriers.data();

    VkDependencyInfo textureShaderReadDepInfo{};
    textureShaderReadDepInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    textureShaderReadDepInfo.imageMemoryBarrierCount = textureShaderReadBarriers.size();
    textureShaderReadDepInfo.pImageMemoryBarriers = textureShaderReadBarriers.data();

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
    renderingInfo.renderArea = {{0, 0}, vkWindow.getExtent()};
    renderingInfo.layerCount = 1;
    renderingInfo.viewMask = 0;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachmentInfo;
    renderingInfo.pDepthAttachment = &depthAttachmentInfo;
    renderingInfo.pStencilAttachment = nullptr;

    vkBeginCommandBuffer(currentCommandBuffer, &beginInfo);
    std::array<VkDeviceSize, 1> offsets = {0};
    VkBuffer buffer = vBuffer.getBuffer();

    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, &buffer, offsets.data());

    vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_,
                            0, dm_.getDescriptorSetSize(), dm_.getDescriptorSet(), 0, nullptr);

    vkCmdPipelineBarrier2(currentCommandBuffer, &depInfo);

    vkCmdPipelineBarrier2(currentCommandBuffer, &textureWriteDepInfo);

    VkImageSubresourceLayers imageSubLayers{};
    imageSubLayers.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageSubLayers.mipLevel = 0;
    imageSubLayers.baseArrayLayer = 0;
    imageSubLayers.layerCount = 1;

    size_t bufferOffset = 0;
    for (size_t i = 0; i < textures.size(); i++) {
        VkBufferImageCopy bufferImageCopy{};
        bufferImageCopy.bufferOffset = bufferOffset;
        bufferImageCopy.bufferRowLength = 0;
        bufferImageCopy.bufferImageHeight = 0;
        bufferImageCopy.imageExtent = textures[i].extent;
        bufferImageCopy.imageOffset = {0, 0, 0};
        bufferImageCopy.imageSubresource = imageSubLayers;

        vkCmdCopyBufferToImage(currentCommandBuffer, stagingBuffer.getBuffer(),
                               vkWindow.getTextureImages()[i].image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

        bufferOffset += textures[i].data.size();
    }

    vkCmdPipelineBarrier2(currentCommandBuffer, &textureShaderReadDepInfo);

    vkCmdBeginRendering(currentCommandBuffer, &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = static_cast<float>(vkWindow.getExtentHeight());
    viewport.width = static_cast<float>(vkWindow.getExtentWidth());
    viewport.height = -static_cast<float>(vkWindow.getExtentHeight());
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vkWindow.getExtent();
    vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);

    if (indexBuffer.getSize() > 0) {
        vkCmdBindIndexBuffer(currentCommandBuffer, indexBuffer.getBuffer(), 0,
                             VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(currentCommandBuffer, indexBuffer.getSize(), 1, 0, 0, 0);
    } else {
        vkCmdDraw(currentCommandBuffer, vBuffer.getSize(), 1, 0, 0);
    }

    vkCmdEndRendering(currentCommandBuffer);
    vkCmdPipelineBarrier2(currentCommandBuffer, &presentDepInfo);

    vkEndCommandBuffer(currentCommandBuffer);
};

void jvk::renderer::Rendering::createSyncObjects(VkDevice device, window::VulkanWindow& vkWindow) {
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT_);
    imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT_);
    renderFinishedSemaphores_.resize(vkWindow.getImages().size());

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; i++) {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                              &imageAvailableSemaphores_[i]) != VK_SUCCESS ||

            vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sync objects");
        }
    }

    for (size_t i = 0; i < vkWindow.getImages().size(); i++) {
        if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                              &renderFinishedSemaphores_[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore");
        }
    }
};
