cmake_minimum_required(VERSION 4.0)

project(jure VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        12.1.0)
FetchContent_MakeAvailable(fmt)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(source src/main.cpp
        src/glfw/initGlfw.cpp

        src/vk/core/instance.cpp
        src/vk/core/physical_device.cpp
        src/vk/core/virtual_device_queue.cpp
        src/vk/core/surface.cpp
        src/vk/core/vk_core.cpp

        src/vk/window/swapchain.cpp
        src/vk/window/imageViews.cpp
        src/vk/window/image.cpp
        src/vk/window/vk_window.cpp
    )

add_executable(jure ${source})
target_include_directories(jure PRIVATE src)
target_link_libraries(jure fmt::fmt Vulkan::Vulkan glfw)
