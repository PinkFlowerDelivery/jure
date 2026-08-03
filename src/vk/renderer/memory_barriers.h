#pragma once

#include <vulkan/vulkan_core.h>

VkImageMemoryBarrier2 createColorBarrier(VkImage currentImage);
VkImageMemoryBarrier2 createDepthBarrier(VkImage depthImage);
VkImageMemoryBarrier2 createPresentBarrier(VkImage currentImage);
VkImageMemoryBarrier2 createTextureWriteBarrier(VkImage textureImage);
VkImageMemoryBarrier2 createTextureShaderReadBarrier(VkImage textureImage);
