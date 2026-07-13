#pragma once

#include "fmt/base.h"
#include "tiny_gltf.h"
#include "vk/resources/vertex_buffer.h"
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

std::pair<std::vector<jure::vk::resources::Vertex>, std::vector<uint32_t>>
vertexLoading(std::string filepath) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    std::vector<jure::vk::resources::Vertex> vertices;
    std::vector<uint32_t> indices;

    bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    if (!warn.empty()) {
        fmt::println("WARN: {}", warn);
    }

    if (!err.empty()) {
        fmt::println("ERR: {}", err);
    }

    if (!res) {
        fmt::println("Failed to load glTF: {}", filepath);
    } else {
        fmt::println("Loaded glTF: {}", filepath);
    }

    for (tinygltf::Mesh& mesh : model.meshes) {
        fmt::println("Primitives count {}", mesh.primitives.size());
        for (tinygltf::Primitive& primitive : mesh.primitives) {
            auto pos = primitive.attributes.find("POSITION");
            if (pos == primitive.attributes.end()) {
                continue;
            }

            if (primitive.indices >= 0) {
                fmt::println("Vertex indices is supported");
                tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];
                tinygltf::BufferView indexBufferView = model.bufferViews[indexAccessor.bufferView];
                tinygltf::Buffer indexBuffer = model.buffers[indexBufferView.buffer];

                indices.reserve(indexAccessor.count);

                const auto* indexDataPtr =
                    indexBuffer.data.data() + indexAccessor.byteOffset + indexBufferView.byteOffset;

                for (size_t i = 0; i < indexAccessor.count; i++) {

                    if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        const auto* buf = reinterpret_cast<const uint16_t*>(indexDataPtr);
                        for (size_t i = 0; i < indexAccessor.count; i++) {
                            indices.push_back(buf[i]);
                        }
                    } else if (indexAccessor.componentType ==
                               TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        const auto* buf = reinterpret_cast<const uint32_t*>(indexDataPtr);
                        for (size_t i = 0; i < indexAccessor.count; ++i) {
                            indices.push_back(buf[i]);
                        }
                    } else if (indexAccessor.componentType ==
                               TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                        const auto* buf = reinterpret_cast<const uint8_t*>(indexDataPtr);
                        for (size_t i = 0; i < indexAccessor.count; ++i) {
                            indices.push_back(buf[i]);
                        }
                    }
                }
            } else {
                fmt::println("Vertex indices is unsupported");
            }

            tinygltf::Accessor accessor = model.accessors[pos->second];
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

                vertices.push_back({{x, y, z}, {1.0f, 0.0f, 0.0f}});

                fmt::println("x: {} y: {} z: {}", x, y, z);
            }

            fmt::println("Vertices size: {}", vertices.size());
            fmt::println("Indices size: {}", indices.size());
            return {vertices, indices};
        }
    }

    return {};
}
