#define GLM_ENABLE_EXPERIMENTAL

#include "obj_loader.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "loaders/model_loader.h"
#include "tiny_obj_loader.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/hash.hpp>
#include <stdexcept>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION

namespace jl = jure::loaders;

std::pair<jl::VertexVector, jl::IndexVector> jl::OBJLoader::load(const std::string& filepath) {
    tinyobj::ObjReaderConfig reader_config;

    auto path = filepath.find_last_of("/");
    auto mtlPath = filepath.substr(0, path);
    reader_config.mtl_search_path = mtlPath;

    std::unordered_map<glm::vec3, uint32_t> indexMap{};
    jl::VertexVector vertices;
    jl::IndexVector indices;

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
    const auto& shapes = reader.GetShapes();

    for (const auto& shape : shapes) {
        size_t indexOffset = 0;

        for (size_t shapeIndex = 0; shapeIndex < shape.mesh.num_face_vertices.size();
             shapeIndex++) {

            auto fv = size_t(shape.mesh.num_face_vertices[shapeIndex]);

            if (fv != 3) {
                throw std::runtime_error("Supported only 3 vertices per face");
            }

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
                glm::vec3 vertex;
                vertex.x = attrib.vertices[(3 * size_t(idx.vertex_index)) + 0];
                vertex.y = attrib.vertices[(3 * size_t(idx.vertex_index)) + 1];
                vertex.z = attrib.vertices[(3 * size_t(idx.vertex_index)) + 2];

                if (indexMap.find(vertex) == indexMap.end()) {
                    vertices.push_back({{vertex.x, vertex.y, vertex.z}, {1.0f, 0.0f, 0.0f}});

                    indexMap[vertex] = vertices.size() - 1;
                }
                indices.push_back(indexMap[vertex]);
            }

            indexOffset += fv;
        };
    }

    return {vertices, indices};
}
