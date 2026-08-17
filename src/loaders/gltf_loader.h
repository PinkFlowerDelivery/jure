#pragma once

#include "loaders/model_loader.h"
#include <glm/fwd.hpp>
#include <tiny_gltf.h>

namespace jure::loaders {
class GLTFLoader : public ModelLoader {
    bool isASCII_;

    void parseIndices(const tinygltf::Model& model, int32_t primitiveIndices, size_t verticesSize,
                      std::vector<uint32_t>& outIndices);
    void parseVertices(tinygltf::Model& model, int32_t position, int32_t texCoords, size_t texIndex,
                       VertexVector& vertices);
    jure::loaders::Texture loadTexture(tinygltf::Model& model, ssize_t texIndex);

  public:
    GLTFLoader(bool isASCII) : isASCII_(isASCII) {};
    Model load(const std::string& filepath) override;
};
} // namespace jure::loaders
