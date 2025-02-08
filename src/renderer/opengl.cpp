#include "opengl.h"

#include <iostream>
#include "core/utils.h"
#include "core/file_system.h"
#include "core/assert.h"

namespace OpenGL 
{

using namespace Engine;

struct LightsUBO 
{
    struct Light 
    {
        glm::vec4 position;
        glm::vec4 direction;
        glm::vec4 color;
        glm::vec4 power_type; // X = power, Y = type
    };
    
    Light lights[MAX_LIGHTS];
};

bool Renderer::initialize() 
{
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) 
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

#ifdef _DEBUG
    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, nullptr);
#endif

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable multisampling if available
    if (GLEW_ARB_multisample) 
    {
        glEnable(GL_MULTISAMPLE);
    }

    // Initialize standard shader
    {
        std::string standardVsCode = FileSystem::read("resources/shaders/standard_vs.glsl");
        std::string standardFsCode = FileSystem::read("resources/shaders/standard_fs.glsl");

        GLuint standardVs = compileShader(standardVsCode, GL_VERTEX_SHADER);
        if (!standardVs) return false;

        GLuint standardFs = compileShader(standardFsCode, GL_FRAGMENT_SHADER);
        if (!standardFs) return false;

        m_standardProgram.id = linkProgram(standardVs, standardFs);
        if (!m_standardProgram.id) return false;
    }

    // Create UBOs
    {
        glCreateBuffers(1, &m_lightsUBO);
        glNamedBufferStorage(m_lightsUBO, sizeof(LightsUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    // Create default texture
    {   
        m_defaultAlbedo = createTexture(DEFAULT_ALBEDO);
        m_defaultNormalMap = createTexture(DEFAULT_NORMAL_MAP);
        m_defaultSpecularMap = createTexture(DEFAULT_SPECULAR_MAP);
    }

    // Create frame buffer
    m_frameBuffer = createFrameBuffer(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, FrameBufferType::Color);
    
    return true;
}

void Renderer::cleanup() 
{
    deleteShader(m_standardProgram.id);
    deleteUniformBuffer(m_lightsUBO);

    deleteTexture(m_defaultAlbedo);
    deleteTexture(m_defaultNormalMap);
    deleteTexture(m_defaultSpecularMap);

    for(auto& [uuid, meshBuffer] : m_meshCache)
    {
        deleteMeshBuffer(meshBuffer);
    }   
    for(auto& [uuid, texture] : m_textureCache)
    {
        deleteTexture(texture);
    }
    
    m_meshCache.clear();
    m_textureCache.clear();

    deleteFrameBuffer(m_frameBuffer);
}

void Renderer::render(std::pair<uint32_t, uint32_t> framebufferSize, Scene& scene)
{
    entt::registry& registry = scene.getRegistry();

    // Allocate meshes and textures
    allocateResources(registry);

    // Update lights uniform buffer
    updateLightsUB(registry);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_lightsUBO);

    // Main Render Pass
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer.id);

    auto [width, height] = framebufferSize;
    if (width == 0 || height == 0) return;
    
    glViewport(0, 0, m_frameBuffer.width, m_frameBuffer.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get active camera
    entt::entity activeCamera = scene.getActiveCamera();
    auto [camera, cameraTransform] = registry.try_get<CameraComponent, TransformComponent>(activeCamera);
    if(!camera || !cameraTransform) return;

    // Camera matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera->fov), 
                                          (float)width/height, 
                                          camera->nearClip, 
                                          camera->farClip);
    glm::mat4 view = glm::lookAt(cameraTransform->position, 
                               cameraTransform->position + MathUtils::forward(*cameraTransform), 
                               MathUtils::up(*cameraTransform));

    // Render all meshes
    auto renderView = registry.view<MeshRendererComponent, TransformComponent>();
    for(auto [entity, mesh, transform] : renderView.each())
    {
        if(!mesh.material) continue;

        glm::mat4 model = MathUtils::calculateModelMatrix(transform);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));
        glm::mat4 modelViewProjection = projection * view * model;

        glUseProgram(m_standardProgram.id);
        
        m_standardProgram.setMat4("viewMatrix", view);
        m_standardProgram.setMat4("modelMatrix", model);
        m_standardProgram.setMat3("normalMatrix", normalMatrix);
        m_standardProgram.setMat4("modelViewProjection", modelViewProjection);
        m_standardProgram.setInt("activeLights", m_activeLights);

        // Bind textures
        if(mesh.material->albedo)
        {
            ASSERT(m_textureCache.contains(mesh.material->albedo->uuid), "Failed to find albedo texture");
            m_standardProgram.setInt("textureAlbedo", 0);
            glBindTextureUnit(0, m_textureCache[mesh.material->albedo->uuid].id);
        }
        else
        {
            m_standardProgram.setInt("textureAlbedo", 0);
            glBindTextureUnit(0, m_defaultAlbedo.id);
        }

        if (mesh.material->normal)
        {
            ASSERT(m_textureCache.contains(mesh.material->normal->uuid), "Failed to find normal map texture");
            m_standardProgram.setInt("textureNormal", 1);
            glBindTextureUnit(1, m_textureCache[mesh.material->normal->uuid].id);
        } 
        else
        {
            m_standardProgram.setInt("textureNormal", 1);
            glBindTextureUnit(1, m_defaultNormalMap.id);
        }

        if (mesh.material->specular)
        {
            ASSERT(m_textureCache.contains(mesh.material->specular->uuid), "Failed to find specular map texture");
            m_standardProgram.setInt("textureSpecular", 2);
            glBindTextureUnit(2, m_textureCache[mesh.material->specular->uuid].id);
        } 
        else
        {
            m_standardProgram.setInt("textureSpecular", 2);
            glBindTextureUnit(2, m_defaultSpecularMap.id);
        }

        m_standardProgram.setVec3("materialAmbient", mesh.material->ambient);
        m_standardProgram.setVec3("specularStrength", mesh.material->specularStrength);
        m_standardProgram.setFloat("shininess", mesh.material->shininess);
        m_standardProgram.setFloat("opacity", mesh.material->opacity);

        // Bind mesh buffer
        ASSERT(m_meshCache.contains(mesh.meshData->uuid), "Failed to find mesh buffer");
        glBindVertexArray(m_meshCache[mesh.meshData->uuid].vao);
        glDrawElements(GL_TRIANGLES, m_meshCache[mesh.meshData->uuid].indexCount, GL_UNSIGNED_INT, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Cleanup
    {
        glBindVertexArray(0);
        glUseProgram(0);
    }
}

void Renderer::allocateResources(entt::registry& registry)
{
    auto allocateResource = [&](const auto& resource, auto& map, auto createFunc) 
    {
        if(resource && !map.contains(resource->uuid))
        {
            map.try_emplace(resource->uuid, (this->*createFunc)(*resource));
        }
    };

    for(auto [entity, mesh] : registry.view<MeshRendererComponent>().each())
    {
        if(!mesh.material) continue;

        allocateResource(mesh.material->albedo, m_textureCache, &Renderer::createTexture);
        allocateResource(mesh.material->normal, m_textureCache, &Renderer::createTexture);
        allocateResource(mesh.material->specular, m_textureCache, &Renderer::createTexture);
        allocateResource(mesh.meshData, m_meshCache, &Renderer::createMeshBuffer);
    };
}

void Renderer::updateLightsUB(entt::registry& registry)
{
    LightsUBO lightsData{};
    auto lightView = registry.view<LightComponent>();
    
    int lightIdx = 0;
    for(auto [entity, light] : lightView.each()) 
    {
        if(lightIdx >= 10) break;

        glm::vec3 lightPos = light.type == LightType::DIRECTIONAL 
                            ? -light.direction * 1000.0f
                            : light.position;

        lightsData.lights[lightIdx] = {
            glm::vec4(lightPos, 1.0f),
            glm::vec4(-light.direction, 0.0f),
            glm::vec4(light.color, 0.0f),
            glm::vec4(light.power, static_cast<float>(light.type), 0.0f, 0.0f)
        };
        
        lightIdx++;
    }
    m_activeLights = lightIdx;

    // Update UBO data
    glNamedBufferSubData(m_lightsUBO, 0, sizeof(LightsUBO), &lightsData);
}

Texture Renderer::createTexture(const Image& image)
{
    Texture texture;
    texture.width = image.width;
    texture.height = image.height;

    glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);
    glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint internalFormat;
    GLenum format;
    switch (image.channels) 
    {
        case 1:
            internalFormat = GL_R8;
            format = GL_RED;
            break;
        case 2:
            internalFormat = GL_RG8;
            format = GL_RG;
            break;
        case 3:
            internalFormat = GL_RGB8;
            format = GL_RGB;
            break;
        case 4:
            internalFormat = GL_RGBA8;
            format = GL_RGBA;
            break;
        default:
            std::cerr << "Unsupported number of channels: " << image.channels << std::endl;
            return texture;
    }

    int levels = MathUtils::calculateNumberOfMipmaps(image.width, image.height);

    glTextureStorage2D(texture.id, levels, internalFormat, texture.width, texture.height);
    glTextureSubImage2D(
        texture.id,
        0,                  // Mip level
        0, 0,               // Offset
        texture.width, 
        texture.height,
        format, 
        GL_UNSIGNED_BYTE, 
        image.pixels.data()
    );
    glGenerateTextureMipmap(texture.id);

    return texture;
}

void Renderer::deleteTexture(Texture& texture)
{
    glDeleteTextures(1, &texture.id);
}

MeshBuffer Renderer::createMeshBuffer(const MeshData& meshData)
{
    MeshBuffer meshBuffer;

    meshBuffer.indexCount = static_cast<uint16_t>(meshData.indices.size());

    glCreateVertexArrays(1, &meshBuffer.vao);
    glCreateBuffers(1, &meshBuffer.vbo);
    glCreateBuffers(1, &meshBuffer.tbo);
    glCreateBuffers(1, &meshBuffer.nbo);
    glCreateBuffers(1, &meshBuffer.tanbo);
    glCreateBuffers(1, &meshBuffer.bitanbo);
    glCreateBuffers(1, &meshBuffer.ebo);

    glNamedBufferData(meshBuffer.vbo, meshData.vertices.size() * sizeof(glm::vec3), 
                     meshData.vertices.data(), GL_STATIC_DRAW);
    glNamedBufferData(meshBuffer.tbo, meshData.uvs.size() * sizeof(glm::vec2), 
                     meshData.uvs.data(), GL_STATIC_DRAW);
    glNamedBufferData(meshBuffer.nbo, meshData.normals.size() * sizeof(glm::vec3), 
                     meshData.normals.data(), GL_STATIC_DRAW);
    glNamedBufferData(meshBuffer.tanbo, meshData.tangents.size() * sizeof(glm::vec3), 
                     meshData.tangents.data(), GL_STATIC_DRAW);
    glNamedBufferData(meshBuffer.bitanbo, meshData.bitangents.size() * sizeof(glm::vec3), 
                     meshData.bitangents.data(), GL_STATIC_DRAW);
    glNamedBufferData(meshBuffer.ebo, meshData.indices.size() * sizeof(uint32_t), 
                     meshData.indices.data(), GL_STATIC_DRAW);

    const auto setupAttrib = [&](GLuint attribIndex, GLuint buffer, GLint size, GLenum type)
    {
        glVertexArrayVertexBuffer(meshBuffer.vao, attribIndex, buffer, 0, size * sizeof(float));
        glVertexArrayAttribFormat(meshBuffer.vao, attribIndex, size, type, GL_FALSE, 0);
        glVertexArrayAttribBinding(meshBuffer.vao, attribIndex, attribIndex);
        glEnableVertexArrayAttrib(meshBuffer.vao, attribIndex);
    };

    setupAttrib(0, meshBuffer.vbo,     3, GL_FLOAT);  // Positions
    setupAttrib(1, meshBuffer.tbo,     2, GL_FLOAT);  // UVs
    setupAttrib(2, meshBuffer.nbo,     3, GL_FLOAT);  // Normals
    setupAttrib(3, meshBuffer.tanbo,   3, GL_FLOAT);  // Tangents
    setupAttrib(4, meshBuffer.bitanbo, 3, GL_FLOAT);  // Bitangents

    glVertexArrayElementBuffer(meshBuffer.vao, meshBuffer.ebo);

    return meshBuffer;
}

void Renderer::deleteMeshBuffer(MeshBuffer& meshBuffer)
{
    meshBuffer.indexCount = 0;
    glDeleteVertexArrays(1, &meshBuffer.vao);
    glDeleteBuffers(1, &meshBuffer.vbo);
    glDeleteBuffers(1, &meshBuffer.tbo);
    glDeleteBuffers(1, &meshBuffer.nbo);
    glDeleteBuffers(1, &meshBuffer.tanbo);
    glDeleteBuffers(1, &meshBuffer.bitanbo);
    glDeleteBuffers(1, &meshBuffer.ebo);
}

FrameBuffer Renderer::createFrameBuffer(int width, int height, FrameBufferType type)
{  
    FrameBuffer fb;
    fb.width = width;
    fb.height = height;
    
    glCreateFramebuffers(1, &fb.id);

    if (type == FrameBufferType::DepthOnly)
    {
        // DEPTH TEXTURE
        glCreateTextures(GL_TEXTURE_2D, 1, &fb.depthTexture);
        glTextureParameteri(fb.depthTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(fb.depthTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(fb.depthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(fb.depthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTextureParameteri(fb.depthTexture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTextureParameteri(fb.depthTexture, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        
        // Important for shadow mapping
        const float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTextureParameterfv(fb.depthTexture, GL_TEXTURE_BORDER_COLOR, borderColor);
        
        glTextureStorage2D(fb.depthTexture, 1, GL_DEPTH_COMPONENT24, width, height);
        glNamedFramebufferTexture(fb.id, GL_DEPTH_ATTACHMENT, fb.depthTexture, 0);

        // No color buffer
        glNamedFramebufferDrawBuffer(fb.id, GL_NONE);
        glNamedFramebufferReadBuffer(fb.id, GL_NONE);
    }
    else
    {
        // COLOR ATTACHMENT
        glCreateTextures(GL_TEXTURE_2D, 1, &fb.colorTexture);
        glTextureParameteri(fb.colorTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(fb.colorTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureStorage2D(fb.colorTexture, 1, GL_RGBA8, width, height);
        glNamedFramebufferTexture(fb.id, GL_COLOR_ATTACHMENT0, fb.colorTexture, 0);

        // DEPTH/STENCIL BUFFER
        GLuint rbo;
        glCreateRenderbuffers(1, &rbo);
        glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, width, height);
        glNamedFramebufferRenderbuffer(fb.id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    if (glCheckNamedFramebufferStatus(fb.id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
    {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        return fb;
    }

    return fb;
}

void Renderer::deleteFrameBuffer(FrameBuffer& fb)
{
    if(fb.colorTexture)
    {
        glDeleteTextures(1, &fb.colorTexture);
        fb.colorTexture = 0;
    }
    if(fb.depthTexture)
    {
        glDeleteTextures(1, &fb.depthTexture);
        fb.depthTexture = 0;
    }
    if(fb.id)
    {
        glDeleteFramebuffers(1, &fb.id);
        fb.id = 0;
    }
    fb.width = fb.height = 0;
}

GLuint Renderer::compileShader(std::string source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) 
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") 
                  << " shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint Renderer::linkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Detach and delete shaders regardless of link success
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Check linking status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

void Renderer::deleteShader(GLuint& shader)
{
    if(shader)
    {
        glDeleteProgram(shader);
        shader = 0;
    }
}

GLuint Renderer::createUniformBuffer(const void* data, size_t size)
{
    GLuint ubo;
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, size, data, GL_DYNAMIC_DRAW);
    return ubo;
}

void Renderer::deleteUniformBuffer(GLuint& buffer)
{
    if(buffer)
    {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
}

#ifdef _DEBUG
void Renderer::debugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    UNUSED(id);
    UNUSED(length);
    UNUSED(userParam);

    // Filter out non-significant error/warning codes
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        return;
    }

    std::cerr << "OpenGL Debug Message:" << std::endl;
    std::cerr << "Source: ";
    switch (source)
    {
        case GL_DEBUG_SOURCE_API: std::cerr << "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cerr << "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cerr << "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cerr << "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: std::cerr << "Other"; break;
    }
    std::cerr << std::endl;

    std::cerr << "Type: ";
    switch (type) 
    {
        case GL_DEBUG_TYPE_ERROR: std::cerr << "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Deprecated Behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "Undefined Behavior"; break;
        case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "Performance"; break;
        case GL_DEBUG_TYPE_MARKER: std::cerr << "Marker"; break;
        case GL_DEBUG_TYPE_OTHER: std::cerr << "Other"; break;
    }
    std::cerr << std::endl;

    std::cerr << "Severity: ";
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: std::cerr << "High"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW: std::cerr << "Low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Notification"; break;
    }
    std::cerr << std::endl;

    std::cerr << "Message: " << message << std::endl << std::endl;

    // Break on high-severity errors
    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        #ifdef _MSC_VER
            __debugbreak();
        #endif
    }
}
#endif

void ShaderProgram::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

} // namespace OpenGL