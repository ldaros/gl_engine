#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera 
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
    ~Camera();

    // Processes input received from any keyboard-like input system
    void processKeyboard(CameraMovement direction, float deltaTime);

    // Processes input received from a mouse input system
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();

private:
    // Calculates the front vector from the Camera's Euler Angles
    void updateCameraVectors();
};

#endif // CAMERA_H
