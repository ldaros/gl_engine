#include "camera.h"
#include <algorithm>

Camera::Camera()
    : m_movementSpeed(2.5f)
    , m_mouseSensitivity(0.1f)
    , m_zoom(45.0f)
{
    m_transform.position = glm::vec3(0.0f, 0.0f, 3.0f); 
    m_transform.rotation = glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f); // Default looking toward -Z
}

Camera::Camera(const glm::vec3& position, float yaw, float pitch)
    : m_movementSpeed(2.5f)
    , m_mouseSensitivity(0.1f)
    , m_zoom(45.0f)
{
    m_transform.position = position;
    m_transform.rotation = glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f);
}

glm::mat4 Camera::getViewMatrix() const 
{
    return glm::lookAt(
        m_transform.position,
        m_transform.position + forward(m_transform),
        up(m_transform)
    );
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) 
{
    float velocity = m_movementSpeed * deltaTime;
    glm::vec3 position = m_transform.position;

    switch (direction)
    {
        case CameraMovement::FORWARD:
            position += forward(m_transform) * velocity;
            break;
        case CameraMovement::BACKWARD:
            position -= forward(m_transform) * velocity;
            break;
        case CameraMovement::LEFT:
            position -= right(m_transform) * velocity;
            break;
        case CameraMovement::RIGHT:
            position += right(m_transform) * velocity;
            break;
        case CameraMovement::UP:
            position += up(m_transform) * velocity;
            break;
        case CameraMovement::DOWN:
            position -= up(m_transform) * velocity;
            break;
    }

    m_transform.position = position;
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;

    // Update rotation angles
    m_rotationAngles.x += xOffset; // Horizontal rotation
    m_rotationAngles.y += yOffset; // Vertical rotation
    
    // Constrain the vertical rotation
    m_rotationAngles.y = std::clamp(m_rotationAngles.y, MIN_PITCH, MAX_PITCH);

    // Create quaternions for horizontal and vertical rotations
    glm::quat horizontalQuat = glm::angleAxis(
        glm::radians(m_rotationAngles.x),
        glm::vec3(0.0f, 1.0f, 0.0f)  // Rotate around Y axis
    );
    
    glm::quat verticalQuat = glm::angleAxis(
        glm::radians(m_rotationAngles.y),
        glm::vec3(1.0f, 0.0f, 0.0f)  // Rotate around X axis
    );

    // Combine rotations and set the transform's rotation
    m_transform.rotation = horizontalQuat * verticalQuat;
}

void Camera::processMouseScroll(float yOffset)
{
    setZoom(m_zoom - yOffset);
}

void Camera::setPosition(const glm::vec3& position)
{
    m_transform.position = position;
}

void Camera::setRotation(float pitch, float yaw)
{
    m_transform.rotation = glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f);
}

void Camera::setZoom(float value)
{
    m_zoom = std::clamp(value, MIN_ZOOM, MAX_ZOOM);
}

void Camera::lookAt(const glm::vec3& target) 
{
    glm::vec3 direction = glm::normalize(target - m_transform.position);
    
    // Calculate pitch and yaw from direction vector
    float pitch = glm::degrees(asin(direction.y));
    float yaw = glm::degrees(atan2(direction.z, direction.x));
    
    m_transform.rotation = glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f);
}