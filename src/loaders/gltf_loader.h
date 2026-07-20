#pragma once

#include "loaders/model_loader.h"
#include <tiny_gltf.h>

namespace jure::loaders {
class GLTFLoader : public ModelLoader {
    bool isASCII_;

    IndexVector parseIndices(tinygltf::Model& model, int32_t primitiveIndices);
    VertexVector parseVertices(tinygltf::Model& model, int32_t position);

  public:
    GLTFLoader(bool isASCII) : isASCII_(isASCII) {};
    std::pair<VertexVector, IndexVector> load(const std::string& filepath) override;
};
} // namespace jure::loaders
