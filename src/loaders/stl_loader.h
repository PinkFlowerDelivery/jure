#pragma once

#include "loaders/model_loader.h"
#include <tiny_gltf.h>

namespace jure::loaders {
class STLLoader : public ModelLoader {
    IndexVector parseIndices(tinygltf::Model& model, int32_t primitiveIndices);
    VertexVector parseVertices(tinygltf::Model& model, int32_t position);

  public:
    std::pair<VertexVector, IndexVector> load(const std::string& filepath) override;
};
} // namespace jure::loaders
