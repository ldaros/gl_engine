#include "opengl.h"
#include "core/assert.h"

namespace OpenGL 
{

bool DebugRenderer::initialize() 
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char* vsCStr = DEBUG_VERTEX_SHADER.c_str();
    glShaderSource(vs, 1, &vsCStr, nullptr);
    glCompileShader(vs);

    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    ASSERT(success, "Debug Vertex shader compilation failed");

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fsCStr = DEBUG_FRAGMENT_SHADER.c_str();
    glShaderSource(fs, 1, &fsCStr, nullptr);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    ASSERT(success, "Debug Fragment shader compilation failed");

    m_debugShader.id = glCreateProgram();
    glAttachShader(m_debugShader.id, vs);
    glAttachShader(m_debugShader.id, fs);
    glLinkProgram(m_debugShader.id);

    glDetachShader(m_debugShader.id, vs);
    glDetachShader(m_debugShader.id, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGetProgramiv(m_debugShader.id, GL_LINK_STATUS, &success);
    ASSERT(success, "Debug Shader program linking failed");

    return true;
}

void DebugRenderer::cleanup() 
{
    if (m_debugShader.id) 
    {
        glDeleteProgram(m_debugShader.id);
        m_debugShader.id = 0;
    }
    m_vertices.clear();
}

void DebugRenderer::startFrame() 
{
    m_vertices.clear();
}

void DebugRenderer::addLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) 
{
    m_vertices.push_back({start, color});
    m_vertices.push_back({end, color});
}

void DebugRenderer::addCube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color) 
{
    glm::vec3 halfSize = size * 0.5f;
    glm::vec3 corners[8] = 
    {
        position + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z),
        position + glm::vec3( halfSize.x, -halfSize.y, -halfSize.z),
        position + glm::vec3( halfSize.x,  halfSize.y, -halfSize.z),
        position + glm::vec3(-halfSize.x,  halfSize.y, -halfSize.z),
        position + glm::vec3(-halfSize.x, -halfSize.y,  halfSize.z),
        position + glm::vec3( halfSize.x, -halfSize.y,  halfSize.z),
        position + glm::vec3( halfSize.x,  halfSize.y,  halfSize.z),
        position + glm::vec3(-halfSize.x,  halfSize.y,  halfSize.z)
    };

    const int edges[12][2] = 
    {
        {0,1}, {1,2}, {2,3}, {3,0}, // Front
        {4,5}, {5,6}, {6,7}, {7,4}, // Back
        {0,4}, {1,5}, {2,6}, {3,7}  // Connections
    };

    for (const auto& edge : edges) 
    {
        addLine(corners[edge[0]], corners[edge[1]], color);
    }
}

void DebugRenderer::addSphere(const glm::vec3& center, float radius, const glm::vec3& color, int segments) 
{
    const float pi = glm::pi<float>();
    const float angleStep = 2.0f * pi / segments;

    // Proper spherical coordinates parameterization
    for (int i = 0; i < segments; ++i) 
    {
        float theta = i * angleStep;
        float nextTheta = (i + 1) * angleStep;
        
        // Longitude lines (vertical circles)
        for (int j = 0; j < segments; ++j) 
        {
            float phi = j * angleStep;
            
            glm::vec3 p1 = center + radius * glm::vec3(
                sin(theta) * cos(phi),
                cos(theta),
                sin(theta) * sin(phi)
            );
            
            glm::vec3 p2 = center + radius * glm::vec3(
                sin(nextTheta) * cos(phi),
                cos(nextTheta),
                sin(nextTheta) * sin(phi)
            );
            
            addLine(p1, p2, color);
        }

        // Latitude lines (horizontal circles)
        for (int j = 0; j < segments; ++j) 
        {
            float phi = j * angleStep;
            
            glm::vec3 p1 = center + radius * glm::vec3(
                sin(phi) * cos(theta),
                cos(phi),
                sin(phi) * sin(theta)
            );
            
            glm::vec3 p2 = center + radius * glm::vec3(
                sin(phi) * cos(nextTheta),
                cos(phi),
                sin(phi) * sin(nextTheta)
            );
            
            addLine(p1, p2, color);
        }
    }
}

void DebugRenderer::endFrame(const glm::mat4& viewProjection) 
{
    if (m_vertices.empty()) return;

    GLuint vao, vbo;
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);

    glNamedBufferStorage(vbo, m_vertices.size() * sizeof(DebugVertex), m_vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(DebugVertex));

    // Position attribute (location 0)
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(DebugVertex, position));
    glVertexArrayAttribBinding(vao, 0, 0);
    glEnableVertexArrayAttrib(vao, 0);

    // Color attribute (location 1)
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(DebugVertex, color));
    glVertexArrayAttribBinding(vao, 1, 0);
    glEnableVertexArrayAttrib(vao, 1);

    glUseProgram(m_debugShader.id);
    m_debugShader.setMat4("uViewProjection", viewProjection);

    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices.size()));
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    m_vertices.clear();
}

} // namespace OpenGL