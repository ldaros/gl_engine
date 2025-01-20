#ifndef TRANSFORM_H
#define TRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform 
{
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};

glm::mat4 getModelMatrix(const Transform& transform);
void translate(Transform& transform, const glm::vec3& translation);
void rotate(Transform& transform, const glm::vec3& eulerAngles);
void scaleUniform(Transform& transform, float uniformScale);
void resetTransform(Transform& transform);
glm::vec3 forward(const Transform& transform);
glm::vec3 right(const Transform& transform);
glm::vec3 up(const Transform& transform);

#endif // TRANSFORM_H
