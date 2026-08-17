#include "gltf_loader.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "loaders/model_loader.h"
#include "vk/resources/structures.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <stdexcept>
#include <tiny_gltf.h>
#include <utility>
#include <vector>

namespace jl = jure::loaders;

template <typename T>
void appendIndices(const uint8_t* dataPtr, size_t count, size_t vertexOffset,
                   std::vector<uint32_t>& outIndices) {
    const auto* buf = reinterpret_cast<const T*>(dataPtr);

    outIndices.reserve(outIndices.size() + count);

    for (size_t i = 0; i < count; i++) {
        outIndices.push_back(static_cast<uint32_t>(buf[i] + vertexOffset));
    }
}

jl::Model jl::GLTFLoader::load(const std::string& filepath) {
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
    }

    std::vector<jure::loaders::Texture> textures(model.textures.size());

    for (size_t i = 0; i < model.textures.size(); ++i) {
        textures[i] = loadTexture(model, i);
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
                parseIndices(model, primitive.indices, vertices.size(), indices);
            }

            auto material = model.materials[primitive.material];
            auto texIndex = material.pbrMetallicRoughness.baseColorTexture.index;
            auto rawColor = material.pbrMetallicRoughness.baseColorFactor;

            if (texIndex == -1 && textures.size() != 16) {
                texIndex = textures.size() + 1;
            }

            parseVertices(model, position->second, texCoords->second, texIndex, vertices);
        }
    }

    return {std::move(vertices), std::move(indices), std::move(textures)};
}

void jl::GLTFLoader::parseIndices(const tinygltf::Model& model, int32_t primitiveIndices,
                                  size_t verticesSize, std::vector<uint32_t>& outIndices) {
    const tinygltf::Accessor& indexAccessor = model.accessors[primitiveIndices];
    const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
    const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

    const auto* indexDataPtr =
        indexBuffer.data.data() + indexAccessor.byteOffset + indexBufferView.byteOffset;

    auto vertexOffset = static_cast<uint32_t>(verticesSize);

    switch (indexAccessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        appendIndices<uint16_t>(indexDataPtr, indexAccessor.count, vertexOffset, outIndices);
        break;

    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        appendIndices<uint32_t>(indexDataPtr, indexAccessor.count, vertexOffset, outIndices);
        break;

    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        appendIndices<uint8_t>(indexDataPtr, indexAccessor.count, vertexOffset, outIndices);
        break;
    }
};

void jl::GLTFLoader::parseVertices(tinygltf::Model& model, int32_t position, int32_t texCoords,
                                   size_t texIndex, VertexVector& vertices) {
    // Texture
    tinygltf::Accessor& tcAccessor = model.accessors[texCoords];
    tinygltf::BufferView& tcBufferView = model.bufferViews[tcAccessor.bufferView];
    tinygltf::Buffer& tcBuffer = model.buffers[tcBufferView.buffer];

    // Vertex
    tinygltf::Accessor& accessor = model.accessors[position];
    tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

    if (accessor.type != TINYGLTF_TYPE_VEC3) {
        throw std::runtime_error("Unsupported vertex position format.");
    }

    auto* dataPtr = buffer.data.data() + accessor.byteOffset + bufferView.byteOffset;
    size_t byteStride = accessor.ByteStride(bufferView);

    auto* tcDataPtr = tcBuffer.data.data() + tcAccessor.byteOffset + tcBufferView.byteOffset;
    size_t tcByteStride = tcAccessor.ByteStride(tcBufferView);

    for (size_t i = 0; i < accessor.count; i++) {
        jure::vk::resources::Vertex vertex;

        const auto* position = reinterpret_cast<const float*>((dataPtr + (i * byteStride)));

        const auto* texCoords = reinterpret_cast<const float*>((tcDataPtr + (i * tcByteStride)));

        vertex.uv = {texCoords[0], texCoords[1]};

        vertex.pos.x = position[0];
        vertex.pos.y = position[1];
        vertex.pos.z = position[2];

        vertex.texIndex = texIndex;

        vertices.push_back(vertex);
    }
};

jure::loaders::Texture jl::GLTFLoader::loadTexture(tinygltf::Model& model, ssize_t texIndex) {

    ssize_t imageIndex = model.textures[texIndex].source;

    if (imageIndex >= 0) {
        const auto& image = model.images[imageIndex];

        // glb format
        if (!image.image.empty()) {
            Texture texture;
            texture.data = image.image;
            texture.extent = {static_cast<uint32_t>(image.width),
                              static_cast<uint32_t>(image.height), 1};
            return texture;
        }

        // gltf format
        if (!image.uri.empty()) {
            return parseTexture(image.uri);
        }
    }

    return {};
};
