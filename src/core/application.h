#pragma once

#include <memory>
#include "sdk.h"
#include "editor/editor_ui.h"
#include "editor/fps_camera.h"

namespace Engine {

class Application
{
public:
    Application() = default;

    bool initialize(int width, int height, const char* title);
    bool loadScene(const std::string& path);
    void run();
    void cleanup();

private:
    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();

    SDK m_sdk;

    std::unique_ptr<Editor::FPSCameraSystem> m_fpsCameraSystem;
    std::unique_ptr<Editor::EditorUI> m_editorUI;

    float m_lastFrameTime = 0.0f;
};

}