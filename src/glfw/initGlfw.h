#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <cstdint>

GLFWwindow* createWindow(int32_t width, int32_t height, const char* title);
