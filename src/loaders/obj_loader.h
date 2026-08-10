
#pragma once

#include "loaders/model_loader.h"
#include "tiny_obj_loader.h"
#include <string>
#include <utility>
#include <vector>

namespace jure::loaders {
class OBJLoader : public ModelLoader {

    std::pair<jure::loaders::VertexVector, jure::loaders::IndexVector>
    parseVerticesAndIndices(std::vector<tinyobj::shape_t> shapes, const tinyobj::attrib_t& attrib);
    std::vector<Texture> loadTextures(const std::vector<tinyobj::material_t>& materials,
                                      const std::string& textureDir);

  public:
    Model load(const std::string& filepath) override;
};
} // namespace jure::loaders
