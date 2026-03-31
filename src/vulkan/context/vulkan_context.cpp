#include "vulkan_context.h"
#include "vulkan/core/imageView.h"
#include "vulkan/core/instance.h"
#include "vulkan/core/physical_device.h"
#include "vulkan/core/queueUtils.h"
#include "vulkan/core/surface.h"
#include "vulkan/core/swapchain.h"
#include "vulkan/core/virtual_device_queue.h"
#include <GLFW/glfw3.h>

VulkanContext createContext(GLFWwindow* window) {
    VulkanContext ctx{};

    ctx.instance = createInstance();
    ctx.surface = createSurface(ctx.instance, window);
    ctx.physicalDevice = pickPhysicalDevice(ctx.instance, ctx.surface);

    QueueFamilyContext queueFamilyContext =
        queueUtils::findQueueFamilies(ctx.physicalDevice, ctx.surface);

    ctx.device = createDevice(ctx.physicalDevice, queueFamilyContext);
    ctx.swapchain =
        createSwapchain(ctx.device, ctx.physicalDevice, ctx.surface, queueFamilyContext, window);

    ctx.imageViews = createImageView(ctx.device, ctx.swapchain);

    return ctx;
};

void destroyContext(VulkanContext& ctx, GLFWwindow* window) {
    glfwDestroyWindow(window);
    vkDestroyInstance(ctx.instance, nullptr);
    vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);
    vkDestroyDevice(ctx.device, nullptr);
}
