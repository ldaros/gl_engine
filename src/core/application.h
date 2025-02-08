#pragma once

#include <memory>
#include "window.h"
#include "resource_manager.h"
#include "scene/scene.h"
#include "ui/ui_manager.h"
#include "renderer/opengl.h"
#include "editor/editor_ui.h"
#include "editor/fps_camera.h"

namespace Engine {

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
    std::unique_ptr<OpenGL::Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<UIManager> m_uiManager;
    std::unique_ptr<ResourceManager> m_resourceManager;

    std::unique_ptr<Editor::FPSCameraSystem> m_fpsCameraSystem;
    std::unique_ptr<Editor::EditorUI> m_editorUI;

    float m_lastFrameTime = 0.0f;
};

}