#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "framebuffer.h"
#include "scene/scene.h"
#include "ui/ui_manager.h"

const std::string DEPTH_VERTEX_SHADER = R"(
#version 410 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(vertexPosition, 1.0);
}
)";

const std::string DEPTH_FRAGMENT_SHADER = R"(
#version 410 core

void main() {}
)";

class Renderer 
{
public:
    bool initialize();
    void toggleWireframe();
    void render(GLFWwindow* window, Scene& scene);

private:
    bool initializeOpenGL();

    void renderShadowMap(Scene& scene);
    void renderScene(Scene& scene, const glm::mat4& view, const glm::mat4& projection);

    void renderMesh(
        Scene& scene, 
        const MeshRendererComponent& meshRenderer, 
        const TransformComponent& transform,
        const glm::mat4& view, 
        const glm::mat4& projection
    );
    
    static void debugCallback(
        GLenum source,
        GLenum type,
        GLuint id, 
        GLenum severity,
        GLsizei length, 
        const GLchar* message,
        const void* userParam
    );

    Shader m_depthShader;
    Framebuffer m_shadowMap{2048, 2048, true, false};
    glm::mat4 m_lightSpaceMatrix;
    const float m_shadowDistance = 20.0f;
    const float m_shadowOrthoSize = 10.0f;
};

#endif // RENDERER_H