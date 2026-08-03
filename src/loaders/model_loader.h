#pragma once

#include "vk/resources/structures.h"
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace jure::loaders {

using VertexVector = std::vector<jure::vk::resources::Vertex>;
using IndexVector = std::vector<uint32_t>;

struct Texture {
    VkExtent3D extent;
    std::vector<uint8_t> data;
};

struct Model {
    VertexVector vertices;
    IndexVector indices;
    std::vector<Texture> textures;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&&) noexcept = default;
    Model& operator=(Model&&) noexcept = default;
};

class ModelLoader {
  protected:
    Texture loadTexture(std::string filepath);

  public:
    virtual Model load(const std::string& filepath) = 0;
    virtual ~ModelLoader() = default;
};
} // namespace jure::loaders
