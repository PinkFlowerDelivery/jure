#include "obj_loader.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "loaders/model_loader.h"
#include "tiny_obj_loader.h"
#include "vk/resources/structures.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#define TINYOBJLOADER_IMPLEMENTATION

namespace jl = jure::loaders;
namespace jvk = jure::vk;

jl::Model jl::OBJLoader::load(const std::string& filepath) {

    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig readerConfig;

    auto lastSlashPos = filepath.find_last_of('/');
    auto mtlSearchPath = filepath.substr(0, lastSlashPos);
    readerConfig.mtl_search_path = mtlSearchPath;

    if (!reader.ParseFromFile(filepath)) {
        if (!reader.Error().empty()) {
            throw std::runtime_error(
                fmt::format("Failed to parse obj model. Error: {}", reader.Error()));
        }
    }

    if (!reader.Warning().empty()) {
        fmt::println("[OBJ] warn: {}", reader.Warning());
    }

    const auto& attrib = reader.GetAttrib();
    const auto& materials = reader.GetMaterials();
    const auto& shapes = reader.GetShapes();

    jl::VertexVector vertices;
    jl::IndexVector indices;
    std::vector<Texture> textures = loadTextures(materials, mtlSearchPath);

    std::unordered_map<jure::vk::resources::Vertex, uint32_t> indexMap{};

    for (const auto& shape : shapes) {
        const auto& mesh = shape.mesh;
        size_t indexOffset = 0;

        for (size_t faceIndex = 0; faceIndex < mesh.num_face_vertices.size(); faceIndex++) {

            size_t vertexCount = mesh.num_face_vertices[faceIndex];
            if (vertexCount > 3) {
                throw std::runtime_error("Supported only 3 vertices per face");
            }

            for (size_t vIdx = 0; vIdx < vertexCount; vIdx++) {
                tinyobj::index_t idx = mesh.indices[indexOffset + vIdx];
                jvk::resources::Vertex vertex;

                vertex.pos.x = attrib.vertices[(3 * idx.vertex_index) + 0];
                vertex.pos.y = attrib.vertices[(3 * idx.vertex_index) + 1];
                vertex.pos.z = attrib.vertices[(3 * idx.vertex_index) + 2];

                if (idx.texcoord_index >= 0) {
                    vertex.uv.x = attrib.texcoords[(2 * idx.texcoord_index) + 0];
                    vertex.uv.y = -attrib.texcoords[(2 * idx.texcoord_index) + 1];
                }

                if (mesh.material_ids[faceIndex] >= 0) {
                    vertex.texIndex = mesh.material_ids[faceIndex];
                }

                vertex.color = {1.0f, 0.0f, 0.0f};

                if (indexMap.find(vertex) == indexMap.end()) {
                    vertices.push_back(vertex);

                    indexMap[vertex] = vertices.size() - 1;
                }
                indices.push_back(indexMap[vertex]);
            }

            indexOffset += vertexCount;
        }
    }

    return {vertices, indices, textures};
}

std::vector<jl::Texture>
jl::OBJLoader::loadTextures(const std::vector<tinyobj::material_t>& materials,
                            const std::string& textureDir) {
    std::vector<jl::Texture> textures;

    for (const auto& material : materials) {
        if (!material.diffuse_texname.empty()) {
            auto texturePath = textureDir + '/' + material.diffuse_texname;

            textures.push_back(loadTexture(texturePath));
        }
    }

    return textures;
};
