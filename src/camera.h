#pragma once

#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

class ArcBallCamera {
    glm::vec3 modelPosition = {0.0f, 0.0f, 0.0f};
    float distance = 5.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float sensivity = 0.5f;

    [[nodiscard]] glm::vec3 getCameraPosition() const {
        glm::vec3 position;
        float r_yaw = glm::radians(yaw);
        float r_pitch = glm::radians(pitch);

        position.x = distance * glm::cos(r_pitch) * glm::sin(r_yaw);
        position.y = distance * glm::sin(r_pitch);
        position.z = distance * glm::cos(r_pitch) * glm::cos(r_yaw);
        return position;
    }

  public:
    glm::mat4x4 getViewMatrix() {
        auto matrix = glm::lookAt(getCameraPosition(), modelPosition, {0.0f, 1.0f, 0.0f});
        return matrix;
    };

    glm::mat4x4 getProjectionMatrix(float width, float height) {
        auto matrix = glm::perspective(glm::radians(45.0f), (width / height), 0.1f, 100.0f);
        return matrix;
    }

    void addYaw(double x) {
        yaw += sensivity * x;
    }

    void addPitch(double y) {

        pitch += sensivity * y;
        pitch = std::clamp(pitch, -89.9f, 89.9f);
    }
};
