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

glm::vec3 Transform::calculateCenter(std::vector<float>& vertices) 
{
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    int vertexCount = vertices.size() / 6; // Assuming each vertex has 6 attributes (position + color)

    for (size_t i = 0; i < vertices.size(); i += 6) {
        center.x += vertices[i];     // X-coordinate
        center.y += vertices[i + 1]; // Y-coordinate
        center.z += vertices[i + 2]; // Z-coordinate
    }

    if (vertexCount > 0) {
        center /= static_cast<float>(vertexCount);
    }

    return center;
}