#ifndef UTILS_H
#define UTILS_H

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include "scene/components.h"

bool calculateTangents(
    const std::vector<glm::vec3> &vertices,
    const std::vector<glm::vec2> &uvs,
    const std::vector<glm::vec3> &normals,
    const std::vector<unsigned int> &indices,
    std::vector<glm::vec3> &out_tangents,
    std::vector<glm::vec3> &out_bitangents
);

glm::mat4 getModelMatrix(const TransformComponent& transform);
void translate(TransformComponent& transform, const glm::vec3& translation);
void rotate(TransformComponent& transform, const glm::vec3& eulerAngles);
void scaleUniform(TransformComponent& transform, float uniformScale);
void resetTransform(TransformComponent& transform);
glm::vec3 forward(const TransformComponent& transform);
glm::vec3 right(const TransformComponent& transform);
glm::vec3 up(const TransformComponent& transform);

#endif // UTILS_H