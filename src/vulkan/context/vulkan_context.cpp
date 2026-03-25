#include "vulkan_context.h"
#include "vulkan/core/instance.h"
#include "vulkan/core/physical_device.h"

VulkanContext createContext() {
    VulkanContext ctx{};

    ctx.instance = createInstance();
    ctx.physicalDevice = pickPhysicalDevice(ctx.instance);

    return ctx;
};
