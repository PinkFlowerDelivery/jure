#include "vk_core.h"
#include "instance.h"
#include "physical_device.h"
#include "surface.h"
#include "virtual_device_queue.h"

void VkCore::init(GLFWwindow* window) {
    instance_ = createInstance();
    surface_ = createSurface(instance_, window);
    physicalDevice_ = pickPhysicalDevice(instance_, surface_);
    device_ = createVirtualDevice(physicalDevice_, surface_);
}
