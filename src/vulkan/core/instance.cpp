#include "instance.h"
#include <cstdint>
#include <cstring>
#include <fmt/base.h>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

bool isValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layer : validationLayers) {
        for (const auto& layerProps : availableLayers) {
            if (strcmp(layer, layerProps.layerName) == 0) {
                return true;
            }
        }
    }
    return false;
}

VkInstance createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "jure";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    if (!isValidationLayerSupport()) {
        fmt::println("Validation layers unsupported");
    };

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledLayerCount = validationLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    instanceCreateInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance.");
    }

    return instance;
};
