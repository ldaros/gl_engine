#pragma once

#include <glm/glm.hpp>
#include "scene/scene.h"

namespace Editor {
    
using namespace Engine;

class FPSCameraSystem
{
public:
    FPSCameraSystem();

    void update(float deltaTime, Scene& scene);

private:
    double m_lastMouseX, m_lastMouseY;
    bool m_firstMouse = true;
    bool m_isRightMouseButtonPressed = false;

    glm::vec2 m_rotationAngles{0.0f};
    float m_movementSpeed = 2.5f;
    float m_mouseSensitivity = 0.1f;
};

}