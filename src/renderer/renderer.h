#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "camera.h"
#include "../scene/scene.h"
#include "../ui/ui_manager.h"

class Renderer {
public:
    bool initialize();
    void render(GLFWwindow* window, Scene& scene, Camera& camera);
    void setupFrame(int width, int height);
    void toggleWireframe();
    
private:
    bool initializeOpenGL();
    bool setupShaders();
    
    static void debugCallback(GLenum source, GLenum type, GLuint id, 
                            GLenum severity, GLsizei length, 
                            const GLchar* message, const void* userParam);

    Shader m_shader;
};

#endif // RENDERER_H