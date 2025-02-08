#include "fps_camera.h"
#include <iostream>
#include "core/input.h"
#include "core/utils.h"

namespace Editor {

using namespace Engine;

FPSCameraSystem::FPSCameraSystem() {}

void FPSCameraSystem::update(Scene& scene, float deltaTime, bool allowMovement)
{
    if (!allowMovement) return;

    entt::registry& registry = scene.getRegistry();

    bool rightMouseButtonCurrentlyPressed = Input::isMouseDown(Mouse::BUTTON_RIGHT);

    if (rightMouseButtonCurrentlyPressed && !m_isRightMouseButtonPressed)
    {
        Input::disableCursor();
        m_isRightMouseButtonPressed = true;
        m_firstMouse = true;
    }
    else if (!rightMouseButtonCurrentlyPressed && m_isRightMouseButtonPressed)
    {
        Input::enableCursor();
        m_isRightMouseButtonPressed = false;
    }

    if (m_isRightMouseButtonPressed)
    {
        auto cameraEntity = scene.getActiveCamera();
        if (cameraEntity == entt::null)
        {
            std::cerr << "MainCamera entity not found!" << std::endl;
            return;
        }

        auto* cameraTransform = registry.try_get<TransformComponent>(cameraEntity);

        // Handle keyboard inputs to move the camera
        float velocity = m_movementSpeed * deltaTime;
        glm::vec3 position = cameraTransform->position;

        if (Input::isKeyDown(Key::W)) 
        {
            cameraTransform->position += MathUtils::forward(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::S))
        {
            cameraTransform->position -= MathUtils::forward(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::D))
        {
            cameraTransform->position += MathUtils::right(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::A))
        {
            cameraTransform->position -= MathUtils::right(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::SPACE))
        {
            cameraTransform->position += MathUtils::up(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::LEFT_SHIFT))
        {
            cameraTransform->position -= MathUtils::up(*cameraTransform) * velocity;
        }

        // Handle mouse movement
        auto mousePosition = Input::getMousePosition();
        if (m_firstMouse) 
        {
            m_lastMouseX = mousePosition.x;
            m_lastMouseY = mousePosition.y;
            m_firstMouse = false;
        }
        double xOffset = m_lastMouseX - mousePosition.x;
        double yOffset = m_lastMouseY - mousePosition.y;
        m_lastMouseX = mousePosition.x;
        m_lastMouseY = mousePosition.y;

        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        // Update rotation angles
        m_rotationAngles.x += static_cast<float>(xOffset); // Horizontal rotation
        m_rotationAngles.y += static_cast<float>(yOffset); // Vertical rotation
    
        // Constrain the vertical rotation
        m_rotationAngles.y = std::clamp(m_rotationAngles.y, -89.0f, 89.0f);

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
        cameraTransform->rotation = horizontalQuat * verticalQuat;
    }
}

}