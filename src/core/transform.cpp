#include "transform.h"

glm::mat4 getModelMatrix(const Transform& transform)
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.position);
    glm::mat4 rotationMatrix = glm::toMat4(transform.rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);
    
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void translate(Transform& transform, const glm::vec3& translation) 
{
    transform.position += translation;
}

void rotate(Transform& transform, const glm::vec3& eulerAngles) 
{
    glm::vec3 radians = glm::radians(eulerAngles);    
    transform.rotation = glm::quat(radians);
}

void scaleUniform(Transform& transform, float uniformScale)
{
    transform.scale = glm::vec3(uniformScale);
}

glm::vec3 forward(const Transform& transform)
{
    return transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 right(const Transform& transform)
{
    return transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 up(const Transform& transform)
{
    return transform.rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}
