#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene/scene.h"

class UIManager 
{
public:
    void initialize(GLFWwindow* window);
    void cleanup();
    void render(Scene& scene);
    void newFrame();
    
    void toggleLightingWindow();
    void togglePerformanceWindow();
    void toggleTransformWindow();

private:
    void renderMainOverlay(Scene& scene);
    void renderLightingWindow(Scene& scene);
    void renderPerformanceWindow();
    void renderTransformWindow(Scene& scene);
    void renderFrame();

    bool m_showLightingWindow = false;
    bool m_showPerformanceWindow = false;
    bool m_showTransformWindow = false;
};

#endif // UI_MANAGER_H