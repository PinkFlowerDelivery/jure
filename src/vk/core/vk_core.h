#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class VkCore {
    VkInstance instance_;
    VkSurfaceKHR surface_;
    VkPhysicalDevice physicalDevice_;
    VkDevice device_;
    VkQueue graphicsQueue_;

  public:
    void init(GLFWwindow* window);
};
