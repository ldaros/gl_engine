#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

#include "../renderer/renderer.h"
#include "../core/window.h"
#include "../scene/scene.h"
#include "../ui/ui_manager.h"

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
    
    float m_lastFrameTime = 0.0f;
    float m_lastMouseX, m_lastMouseY;
    bool m_firstMouse = true;
    bool m_lightTogglePressed = false;
    bool m_performanceTogglePressed = false;
    bool m_wireframeTogglePressed = false;
    bool m_transformTogglePressed = false;
    bool m_isRightMouseButtonPressed = false;
};


#endif // APPLICATION_H