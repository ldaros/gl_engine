#ifndef CAMERA_H
#define CAMERA_H

#include "core/transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera 
{
public:
    Camera();
    Camera(const glm::vec3& position, float yaw = -90.0f, float pitch = 0.0f);

    // Core functionality
    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const { return m_transform.position; }
    glm::vec3 getForward() const { return forward(m_transform); }
    glm::vec3 getRight() const { return right(m_transform); }
    glm::vec3 getUp() const { return up(m_transform); }

    // Camera control
    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    void processMouseScroll(float yOffset);

    // Camera properties
    void setPosition(const glm::vec3& position);
    void setRotation(float pitch, float yaw);
    
    // Camera parameters
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    void setZoom(float value);
    float getZoom() const { return m_zoom; }

    // Utility functions
    void lookAt(const glm::vec3& target);

private:
    Transform m_transform;
    glm::vec2 m_rotationAngles{0.0f};
    
    // Camera parameters
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

    // Constraints
    static constexpr float MIN_ZOOM = 1.0f;
    static constexpr float MAX_ZOOM = 45.0f;
    static constexpr float MIN_PITCH = -89.0f;
    static constexpr float MAX_PITCH = 89.0f;
};

#endif // CAMERA_H