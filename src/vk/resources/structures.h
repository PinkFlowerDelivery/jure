#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/mat4x4.hpp>

namespace jure::vk::resources {

struct ArcBallCameraUniform {
    glm::mat4x4 view;
    glm::mat4x4 proj;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv = {0.0f, 0.0f};
    size_t texIndex = 0;

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && uv == other.uv &&
               texIndex == other.texIndex;
    }
};

} // namespace jure::vk::resources

namespace std {
template <> struct hash<jure::vk::resources::Vertex> {
    size_t operator()(const jure::vk::resources::Vertex& vertex) const {
        size_t h1 = hash<glm::vec3>{}(vertex.pos);
        size_t h2 = hash<glm::vec3>{}(vertex.color);
        size_t h3 = hash<glm::vec2>{}(vertex.uv);
        size_t h4 = hash<size_t>{}(vertex.texIndex);

        size_t seed = h1;
        seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
} // namespace std
