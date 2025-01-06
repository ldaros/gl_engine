#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction)
    : position(position), direction(direction) {}

glm::mat4 Camera::getViewMatrix() const 
{
    return glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
}

glm::mat4 Camera::getProjectionMatrix(float fov, float aspectRatio, float near, float far) const 
{
    return glm::perspective(fov, aspectRatio, near, far);
}

glm::vec3 Camera::getPosition() const 
{
    return position;
}

glm::vec3 Camera::getDirection() const 
{
    return direction;
}

void Camera::setPosition(glm::vec3 position) 
{
    this->position = position;
}

void Camera::setDirection(glm::vec3 direction) 
{
    this->direction = direction;
}