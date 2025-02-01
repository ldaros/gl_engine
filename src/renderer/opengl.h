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

const int MAX_LIGHTS = 10;

const Image DEFAULT_TEXTURE = {
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

class Renderer 
{
public:
    bool initialize();
    void cleanup();
    void render(Window& window, Scene& scene);
    
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
    
    GLuint m_lightsUBO;
    uint32_t m_activeLights;

    ShaderProgram m_standardProgram;
    Texture m_defaultTexture,  m_defaultNormalMap;

    std::map<UUID, MeshBuffer> m_meshCache;
    std::map<UUID, Texture> m_textureCache;
};

} // namespace OpenGL
