#include "vulkan_context.h"
#include "glfw/initGlfw.h"
#include "vulkan/core/instance.h"
#include "vulkan/core/physical_device.h"
#include "vulkan/core/surface.h"
#include "vulkan/core/virtual_device_queue.h"
#include <GLFW/glfw3.h>

VulkanContext createContext(GLFWwindow* window) {
    VulkanContext ctx{};

    ctx.instance = createInstance();
    ctx.surface = createSurface(ctx.instance, window);
    ctx.physicalDevice = pickPhysicalDevice(ctx.instance, ctx.surface);
    ctx.device = createDevice(ctx.physicalDevice, ctx.surface);

    return ctx;
};

void destroyContext(VulkanContext& ctx, GLFWwindow* window) {
    glfwDestroyWindow(window);
    vkDestroyInstance(ctx.instance, nullptr);
    vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);
    vkDestroyDevice(ctx.device, nullptr);
}
