#include "transform.h"

Transform::Transform() : modelMatrix(1.0f) {}

glm::mat4 Transform::getModelMatrix() const {
    return modelMatrix;
}

void Transform::rotate(float angle, glm::vec3 axis) {
    modelMatrix = glm::rotate(modelMatrix, angle, axis);
}