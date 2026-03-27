#include "glfw/initGlfw.h"
#include "vulkan/context/vulkan_context.h"
#include <GLFW/glfw3.h>

int main() {
    GLFWwindow* window = createWindow(640, 480, "vk");

    VulkanContext ctx = createContext(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}
