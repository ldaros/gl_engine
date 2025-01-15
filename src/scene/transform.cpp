#include "transform.h"

Transform::Transform() : modelMatrix(1.0f) {}

glm::mat4 Transform::getModelMatrix() const 
{
    return modelMatrix;
}

void Transform::rotate(float angle, glm::vec3 axis)
{
    modelMatrix = glm::rotate(modelMatrix, angle, axis);
}

void Transform::translate(glm::vec3 translation)
{
    modelMatrix = glm::translate(modelMatrix, translation);
}

void Transform::scale(glm::vec3 scale)
{
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Transform::reset()
{
    modelMatrix = glm::mat4(1.0f);
}
