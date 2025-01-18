#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "framebuffer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "ui/ui_manager.h"

class Renderer 
{
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

    void renderShadowMap(Scene& scene);

    Shader m_shader;
    Shader m_depthShader;
    Framebuffer m_shadowMap{2048, 2048, true, false};
    glm::mat4 m_lightSpaceMatrix;
    const float m_shadowDistance = 10.0f;
};

#endif // RENDERER_H