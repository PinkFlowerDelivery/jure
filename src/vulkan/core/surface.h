#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window);
