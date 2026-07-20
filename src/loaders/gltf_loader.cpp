#include "gltf_loader.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "loaders/model_loader.h"
#include <stdexcept>
#include <tiny_gltf.h>
#include <utility>
#include <vector>

namespace jl = jure::loaders;
std::pair<jl::VertexVector, jl::IndexVector> jl::GLTFLoader::load(const std::string& filepath) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    jl::VertexVector vertices;
    jl::IndexVector indices;

    bool res;

    if (isASCII_) {
        res = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
    } else {
        res = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    }

    if (!err.empty()) {
        throw std::runtime_error(fmt::format("Failed to load gltf model. Error: {}", err));
    }
    if (!warn.empty()) {
        fmt::println("WARN: {}", warn);
    }

    if (!res) {
        fmt::println("Failed to load glTF model: {}", filepath);
    } else {
        fmt::println("Loaded glTF model: {}", filepath);
    }

    for (tinygltf::Mesh& mesh : model.meshes) {

        for (tinygltf::Primitive& primitive : mesh.primitives) {
            auto position = primitive.attributes.find("POSITION");
            if (position == primitive.attributes.end()) {
                continue;
            }

            if (primitive.indices != -1) {
                fmt::println("Vertex indices is supported");
                indices = parseIndices(model, primitive.indices);

            } else {
                fmt::println("Vertex indices is unsupported");
            }

            vertices = parseVertices(model, position->second);
        }
    }

    return {vertices, indices};
}

jl::VertexVector jl::GLTFLoader::parseVertices(tinygltf::Model& model, int32_t position) {
    jl::VertexVector vertices;

    tinygltf::Accessor accessor = model.accessors[position];
    tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

    if (accessor.type != TINYGLTF_TYPE_VEC3) {
        throw std::runtime_error("Unsupported vertex position format.");
    }

    auto* dataPtr = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;

    size_t byteStride = accessor.ByteStride(bufferView);

    for (size_t i = 0; i < accessor.count; i++) {
        const auto* position = reinterpret_cast<const float*>((dataPtr + (i * byteStride)));

        float x = position[0];
        float y = position[1];
        float z = position[2];

        vertices.push_back({{x, y, z}, {61.0f, 61.0f, 61.0f}});
    }

    return vertices;
};

jl::IndexVector jl::GLTFLoader::parseIndices(tinygltf::Model& model, int32_t primitiveIndices) {
    jl::IndexVector indices;

    tinygltf::Accessor indexAccessor = model.accessors[primitiveIndices];
    tinygltf::BufferView indexBufferView = model.bufferViews[indexAccessor.bufferView];
    tinygltf::Buffer indexBuffer = model.buffers[indexBufferView.buffer];

    indices.reserve(indexAccessor.count);

    const auto* indexDataPtr =
        indexBuffer.data.data() + indexAccessor.byteOffset + indexBufferView.byteOffset;

    for (size_t i = 0; i < indexAccessor.count; i++) {

        if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            const auto* buf = reinterpret_cast<const uint16_t*>(indexDataPtr);
            indices.push_back(buf[i]);
        } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            const auto* buf = reinterpret_cast<const uint32_t*>(indexDataPtr);
            indices.push_back(buf[i]);
        } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
            const auto* buf = reinterpret_cast<const uint8_t*>(indexDataPtr);
            indices.push_back(buf[i]);
        }
    }

    return indices;
};
