#include "vulkan_context.h"
#include "vulkan/core/image.h"
#include "vulkan/core/imageView.h"
#include "vulkan/core/instance.h"
#include "vulkan/core/physical_device.h"
#include "vulkan/core/surface.h"
#include "vulkan/core/swapchain.h"
#include "vulkan/core/virtual_device_queue.h"
#include "vulkan/utils/vulkan_helpers.h"
#include <GLFW/glfw3.h>

VulkanContext createContext(GLFWwindow* window) {
    VulkanContext ctx{};

    ctx.instance = createInstance();
    ctx.surface = createSurface(ctx.instance, window);
    ctx.physicalDevice = pickPhysicalDevice(ctx.instance, ctx.surface);

    QueueFamilyContext queueFamilyContext =
        vulkan_helpers::findQueueFamilies(ctx.physicalDevice, ctx.surface);

    ctx.device = createDevice(ctx.physicalDevice, queueFamilyContext);

    SwapchainDetails swapchain =
        createSwapchain(ctx.device, ctx.physicalDevice, ctx.surface, queueFamilyContext, window);

    ctx.swapchain = swapchain.swapchain;

    DepthImage depthImage = createImage(ctx.physicalDevice, ctx.device, swapchain.extent);

    ctx.imageViews = createImageView(ctx.physicalDevice, ctx.device, ctx.swapchain,
                                     depthImage.image, swapchain.imageFormat);

    return ctx;
};

void destroyContext(VulkanContext& ctx, GLFWwindow* window) {
    glfwDestroyWindow(window);
    vkDestroyInstance(ctx.instance, nullptr);
    vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);
    vkDestroyDevice(ctx.device, nullptr);
}
