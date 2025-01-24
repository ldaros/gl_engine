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
    
private:
    void renderPerformanceOverlay();
    void renderEntityInfo(Scene& scene);
    void renderFrame();

    entt::entity m_selectedEntity = entt::null;
};

#endif // UI_MANAGER_H