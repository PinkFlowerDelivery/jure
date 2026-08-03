#include "gltf_loader.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "loaders/model_loader.h"
#include "vk/resources/structures.h"
#include <glm/fwd.hpp>
#include <stdexcept>
#include <tiny_gltf.h>
#include <utility>
#include <vector>

namespace jl = jure::loaders;

jl::Model jl::GLTFLoader::load(const std::string& filepath) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    jl::VertexVector vertices;
    jl::IndexVector indices;
    std::vector<jure::loaders::Texture> textures;

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
            auto texCoords = primitive.attributes.find("TEXCOORD_0");

            if (texCoords == primitive.attributes.end()) {
                texCoords->second = 0;
            }

            if (position == primitive.attributes.end()) {
                continue;
            }

            if (primitive.indices != -1) {
                fmt::println("Vertex indices is supported");
                indices = parseIndices(model, primitive.indices);
            }

            ssize_t texIndex = 0;

            if (model.textures.size() > 0) {
                auto material = model.materials[primitive.material];
                textures.push_back(loadTextures(model, material, texIndex));
            }

            vertices = parseVertices(model, position->second, texCoords->second, texIndex);
        }
    }

    return {vertices, indices, textures};
}

jure::loaders::Texture jl::GLTFLoader::loadTextures(tinygltf::Model& model,
                                                    tinygltf::Material material,
                                                    ssize_t& texIndex) {
    if (material.pbrMetallicRoughness.baseColorTexture.index != -1) {
        texIndex = material.pbrMetallicRoughness.baseColorTexture.index;

        ssize_t imageIndex = 0;

        if (texIndex >= 0 && texIndex < static_cast<ssize_t>(model.textures.size())) {
            imageIndex = model.textures[texIndex].source;
        }

        if (imageIndex >= 0) {
            const auto& image = model.images[imageIndex];

            if (!image.image.empty()) {
                Texture texture;
                texture.data = image.image;
                texture.extent = {static_cast<uint32_t>(image.width),
                                  static_cast<uint32_t>(image.height), 1};
                return texture;
            }

            if (!image.uri.empty()) {
                return loadTexture(image.uri);
            }
        }
    }

    return {};
};

jl::VertexVector jl::GLTFLoader::parseVertices(tinygltf::Model& model, int32_t position,
                                               int32_t texIndex, size_t index) {
    jl::VertexVector vertices;
    jure::vk::resources::Vertex vertex;

    tinygltf::Accessor texAccessor = model.accessors[texIndex];
    tinygltf::BufferView texBufferView = model.bufferViews[texAccessor.bufferView];
    tinygltf::Buffer texBuffer = model.buffers[texBufferView.buffer];

    tinygltf::Accessor accessor = model.accessors[position];
    tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

    if (accessor.type != TINYGLTF_TYPE_VEC3) {
        throw std::runtime_error("Unsupported vertex position format.");
    }

    auto* texDataPtr = texBuffer.data.data() + texAccessor.byteOffset + texBufferView.byteOffset;
    size_t texByteStride = texAccessor.ByteStride(texBufferView);

    auto* dataPtr = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;
    size_t byteStride = accessor.ByteStride(bufferView);

    for (size_t i = 0; i < accessor.count; i++) {
        const auto* position = reinterpret_cast<const float*>((dataPtr + (i * byteStride)));

        const auto* texCoords = reinterpret_cast<const float*>((texDataPtr + (i * texByteStride)));

        vertex.uv = {texCoords[0], texCoords[1]};

        vertex.pos.x = position[0];
        vertex.pos.y = position[1];
        vertex.pos.z = position[2];

        vertex.texIndex = index;

        vertices.push_back(vertex);
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
