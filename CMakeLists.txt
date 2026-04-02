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

add_executable(jure src/main.cpp src/vulkan/core/instance.cpp src/vulkan/context/vulkan_context.cpp src/vulkan/core/physical_device.cpp src/vulkan/core/virtual_device_queue.cpp src/glfw/initGlfw.cpp src/vulkan/core/surface.cpp src/vulkan/utils/vulkan_helpers.cpp src/vulkan/core/swapchain.cpp src/vulkan/core/imageView.cpp src/vulkan/core/image.cpp)
target_include_directories(jure PRIVATE src)
target_link_libraries(jure fmt::fmt Vulkan::Vulkan glfw)
