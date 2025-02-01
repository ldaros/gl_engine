#include "utils.h"

namespace Engine {

glm::mat4 MathUtils::calculateModelMatrix(const TransformComponent& transform)
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.position);
    glm::mat4 rotationMatrix = glm::toMat4(transform.rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);
    
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void MathUtils::translate(TransformComponent& transform, const glm::vec3& translation) 
{
    transform.position += translation;
}

void MathUtils::rotate(TransformComponent& transform, const glm::vec3& eulerAngles) 
{
    glm::vec3 radians = glm::radians(eulerAngles);    
    transform.rotation = glm::quat(radians);
}

void MathUtils::scaleUniform(TransformComponent& transform, float uniformScale)
{
    transform.scale = glm::vec3(uniformScale);
}

glm::vec3 MathUtils::forward(const TransformComponent& transform)
{
    return transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 MathUtils::right(const TransformComponent& transform)
{
    return transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 MathUtils::up(const TransformComponent& transform)
{
    return transform.rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

int MathUtils::calculateNumberOfMipmaps(int width, int height)
{
    int max_dim = std::max(width, height);
    int levels = static_cast<int>(std::log2(max_dim)) + 1;

    return levels;
}

}
