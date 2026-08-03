#pragma once

#include "loaders/model_loader.h"
#include <glm/fwd.hpp>
#include <tiny_gltf.h>

namespace jure::loaders {
class GLTFLoader : public ModelLoader {
    bool isASCII_;

    IndexVector parseIndices(tinygltf::Model& model, int32_t primitiveIndices);
    VertexVector parseVertices(tinygltf::Model& model, int32_t position, int32_t texIndex,
                               size_t index);
    jure::loaders::Texture loadTextures(tinygltf::Model& model, tinygltf::Material material,
                                        ssize_t& texIndex);

  public:
    GLTFLoader(bool isASCII) : isASCII_(isASCII) {};
    Model load(const std::string& filepath) override;
};
} // namespace jure::loaders
