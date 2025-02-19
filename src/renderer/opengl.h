#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/uuid.h"
#include "core/window.h"
#include "core/resources.h"
#include "scene/scene.h"

namespace OpenGL 
{

using namespace Engine;

const uint16_t MAX_LIGHTS = 10;
const uint16_t FRAMEBUFFER_WIDTH = 1920;
const uint16_t FRAMEBUFFER_HEIGHT = 1080;

const Image DEFAULT_ALBEDO = {
    .pixels = {245, 245, 245}, 
    .uuid = 0,
    .width = 1, 
    .height = 1, 
    .channels = 3,
};

const Image DEFAULT_NORMAL_MAP = {
    .pixels = {128, 128, 255}, 
    .uuid = 0,
    .width = 1,
    .height = 1,
    .channels = 3,
};

const Image DEFAULT_SPECULAR_MAP = {
    .pixels = {255, 255, 255},
    .uuid = 0,
    .width = 1,
    .height = 1,
    .channels = 3,
};

const std::string DEBUG_VERTEX_SHADER = R"(
#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 uViewProjection;

out vec3 vColor;

void main() {
    gl_Position = uViewProjection * vec4(aPos, 1.0);
    vColor = aColor;
}
)";

const std::string DEBUG_FRAGMENT_SHADER = R"(
#version 450 core

in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

struct Texture
{
    GLuint id;
    int width, height;
    int levels;
};

struct MeshBuffer
{
    uint16_t indexCount;
    GLuint vao, vbo, tbo, nbo, tanbo, bitanbo, ebo;
};

enum class FrameBufferType 
{
    Color,           // Color texture + depth/stencil buffer
    DepthOnly        // Depth texture only
};

struct FrameBuffer
{
    GLuint id;
    GLuint colorTexture, depthTexture;
    int width, height;
};

class ShaderProgram
{
public:
    GLuint id;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};
    
class DebugRenderer
{
public:
    bool initialize();
    void cleanup();
    void startFrame();
    void addLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    void addCube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color);
    void addSphere(const glm::vec3& position, float radius, const glm::vec3& color, int segments = 20);
    void endFrame(const glm::mat4& viewProjection);

private:
    struct DebugVertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    ShaderProgram m_debugShader;
    std::vector<DebugVertex> m_vertices;
};

class Renderer 
{
public:
    bool initialize();
    void cleanup();
    void render(std::pair<uint32_t, uint32_t> framebufferSize, Scene& scene);
    void toggleDebug(bool enabled) { m_debugEnabled = enabled; };
    FrameBuffer getFrameBuffer() const { return m_frameBuffer; };
    
private:
    Texture createTexture(const Image& image);
    void deleteTexture(Texture& texture);

    MeshBuffer createMeshBuffer(const MeshData& meshData);
    void deleteMeshBuffer(MeshBuffer& meshBuffer);

    FrameBuffer createFrameBuffer(int width, int height, FrameBufferType type);
    void deleteFrameBuffer(FrameBuffer& frameBuffer);

    GLuint compileShader(std::string source, GLenum type);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    void deleteShader(GLuint& shader);

    GLuint createUniformBuffer(const void* data, size_t size);
    void deleteUniformBuffer(GLuint& buffer);

#ifdef _DEBUG
    static void debugCallback(
        GLenum source,
        GLenum type,
        GLuint id, 
        GLenum severity,
        GLsizei length, 
        const GLchar* message,
        const void* userParam
    );
#endif
    void allocateResources(entt::registry& registry);
    void updateLightsUB(entt::registry& registry);
    
    bool m_debugEnabled = false;

    GLuint m_lightsUBO;
    uint32_t m_activeLights;

    ShaderProgram m_standardProgram;
    Texture m_defaultAlbedo,  m_defaultNormalMap, m_defaultSpecularMap;
    
    std::map<UUID, MeshBuffer> m_meshCache;
    std::map<UUID, Texture> m_textureCache;

    FrameBuffer m_frameBuffer;
    DebugRenderer m_debugRenderer;
};

} // namespace OpenGL
