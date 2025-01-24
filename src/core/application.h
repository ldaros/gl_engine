#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "core/window.h"
#include "scene/scene.h"
#include "ui/ui_manager.h"
#include "renderer/renderer.h"
#include "renderer/material.h"
#include "renderer/mesh.h"

class Application
{
public:
    Application();
    ~Application();

    bool initialize(int width, int height, const char* title);
    void run();
    void cleanup();

private:
    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<UIManager> m_uiManager;
    
    std::shared_ptr<Texture> m_diffuseTexture;
    std::shared_ptr<Texture> m_normalMap;
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Mesh> m_mesh2;
    std::shared_ptr<Material> m_material;
    
    float m_lastFrameTime = 0.0f;
    float m_lastMouseX, m_lastMouseY;
    bool m_firstMouse = true;
    bool m_wireframeTogglePressed = false;
    bool m_isRightMouseButtonPressed = false;

    glm::vec2 m_rotationAngles{0.0f};
    float m_movementSpeed = 2.5f;
    float m_mouseSensitivity = 0.1f;
};


#endif // APPLICATION_H