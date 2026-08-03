#include "obj_loader.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "loaders/model_loader.h"
#include "tiny_obj_loader.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#define TINYOBJLOADER_IMPLEMENTATION

namespace jl = jure::loaders;

jl::Model jl::OBJLoader::load(const std::string& filepath) {
    tinyobj::ObjReaderConfig reader_config;

    auto path = filepath.find_last_of("/");
    auto mtlPath = filepath.substr(0, path);
    reader_config.mtl_search_path = mtlPath;

    std::vector<jure::loaders::Texture> textures;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath)) {
        if (!reader.Error().empty()) {
            throw std::runtime_error(
                fmt::format("Failed to parse obj model. Err: {}", reader.Error()));
        }
    }

    if (!reader.Warning().empty()) {
        fmt::println("Obj parser warn: {}", reader.Warning());
    }

    const auto& attrib = reader.GetAttrib();
    const auto& materials = reader.GetMaterials();
    const auto& shapes = reader.GetShapes();

    for (const auto& material : materials) {
        if (!material.diffuse_texname.empty()) {
            auto texturePath = mtlPath + '/' + material.diffuse_texname;

            textures.push_back(loadTexture(texturePath));
        }
    }

    auto [vertices, indices] = parseVerticesAndIndices(shapes, attrib);

    return {std::move(vertices), std::move(indices), std::move(textures)};
}

std::pair<jl::VertexVector, jl::IndexVector>
jl::OBJLoader::parseVerticesAndIndices(const std::vector<tinyobj::shape_t> shapes,
                                       const tinyobj::attrib_t& attrib) {
    std::unordered_map<jure::vk::resources::Vertex, uint32_t> indexMap{};
    jl::VertexVector vertices;
    jl::IndexVector indices;

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t indexOffset = 0;

        for (size_t shapeIndex = 0; shapeIndex < shapes[s].mesh.num_face_vertices.size();
             shapeIndex++) {

            auto fv = size_t(shapes[s].mesh.num_face_vertices[shapeIndex]);

            if (fv != 3) {
                throw std::runtime_error("Supported only 3 vertices per face");
            }

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
                jure::vk::resources::Vertex vertex;
                vertex.color = {1.0f, 0.0f, 0.0f};

                vertex.pos.x = attrib.vertices[(3 * size_t(idx.vertex_index)) + 0];
                vertex.pos.y = attrib.vertices[(3 * size_t(idx.vertex_index)) + 1];
                vertex.pos.z = attrib.vertices[(3 * size_t(idx.vertex_index)) + 2];

                if (idx.texcoord_index >= 0) {
                    vertex.uv.x = attrib.texcoords[(2 * size_t(idx.texcoord_index)) + 0];
                    vertex.uv.y = -attrib.texcoords[(2 * size_t(idx.texcoord_index)) + 1];
                }

                if (shapes[s].mesh.material_ids[s] >= 0) {
                    vertex.texIndex = shapes[s].mesh.material_ids[s];
                }

                if (indexMap.find(vertex) == indexMap.end()) {
                    vertices.push_back(vertex);

                    indexMap[vertex] = vertices.size() - 1;
                }
                indices.push_back(indexMap[vertex]);
            }

            indexOffset += fv;
        };
    }

    return {vertices, indices};
};
