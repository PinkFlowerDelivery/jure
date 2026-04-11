#include "glfw/initGlfw.h"
#include "vk/core/vk_core.h"
#include "vk/window/vk_window.h"
#include <GLFW/glfw3.h>

int main() {
    GLFWwindow* window = createWindow(640, 480, "vk");

    VulkanCore core(window);
    VulkanWindow vkWindow(core, window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}
