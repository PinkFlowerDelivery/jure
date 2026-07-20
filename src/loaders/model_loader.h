#pragma once

#include "vk/resources/vertex_buffer.h"
#include <string>
#include <utility>
#include <vector>

namespace jure::loaders {

using VertexVector = std::vector<jure::vk::resources::Vertex>;
using IndexVector = std::vector<uint32_t>;

class ModelLoader {
  public:
    virtual std::pair<VertexVector, IndexVector> load(const std::string& filepath) = 0;
    virtual ~ModelLoader() = default;
};
} // namespace jure::loaders
