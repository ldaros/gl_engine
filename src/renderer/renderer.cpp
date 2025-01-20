#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "core/utils.h"
#include "core/window.h"
#include "core/file_system.h"

bool Renderer::initialize()
{
    if (!initializeOpenGL())
    {
        return false;
    }
    
    if (!setupShaders())
    {
        return false;
    }
       
    if (!m_shadowMap.init())
    {
        return false;
    }

    return true;
}

bool Renderer::initializeOpenGL() 
{
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    // Setup OpenGL debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, nullptr);

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
    if (GLEW_ARB_multisample) {
        glEnable(GL_MULTISAMPLE);
    }

    return true;
}

bool Renderer::setupShaders() 
{
    // Load and compile shaders
    if (!m_depthShader.init(
            FileSystem::read("resources/shaders/depth_vs.glsl"),
            FileSystem::read("resources/shaders/depth_fs.glsl"))) 
    {
        std::cerr << "Failed to initialize depth shader" << std::endl;
        return false;
    }
    return true;
}

void Renderer::renderShadowMap(Scene& scene)
{
    const Light& light = scene.getLight();
    
    // Only render shadow map for directional lights
    if (!light.castsShadows())
    {
        return;
    }

    m_shadowMap.bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    
    float orthoSize = 10.0f;
    glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, m_shadowDistance * 2.0f);
    
    // Use light direction for directional light
    glm::vec3 lightDir = light.getDirection();
    glm::vec3 lightPos = -lightDir * m_shadowDistance;  // Position light far along its direction
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
    m_lightSpaceMatrix = lightProjection * lightView;
    
    m_depthShader.use();
    m_depthShader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
    m_depthShader.setMat4("model", getModelMatrix(scene.getTransform()));

    scene.getMesh().draw();
    
    m_shadowMap.unbind();
}

void Renderer::setupFrame(int width, int height) 
{
    // Update viewport and clear buffers
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void Renderer::render(GLFWwindow* window, Scene& scene, Camera& camera) {
    const Mesh& mesh = scene.getMesh();
    const Transform& transform = scene.getTransform();
    const Light& light = scene.getLight();
    const Material& material = scene.getMaterial();
    const Shader& shader = *material.shader;

    // Render shadow map
    renderShadowMap(scene);

    // Get window size for aspect ratio
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); // Reset viewport

    // Calculate matrices
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getZoom()),
        float(width) / float(height),
        0.1f,
        100.0f
    );
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 model = getModelMatrix(transform);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

    // Use shader and set uniforms
    shader.use();
    shader.setMat4("modelViewProjection", projection * view * model);
    shader.setMat4("viewMatrix", view);
    shader.setMat4("modelMatrix", model);
    shader.setMat3("normalMatrix", normalMatrix);
    shader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);

    // Set light uniforms
    shader.setVec3("lightPosition", light.getPosition());
    shader.setFloat("lightPower", light.getPower());
    shader.setVec3("lightColor", light.getColor());
    shader.setBool("isDirectionalLight", light.getType() == LightType::DIRECTIONAL);

    if (scene.getLight().getType() == LightType::DIRECTIONAL)
    {
        shader.setVec3("lightPosition", -scene.getLight().getDirection() * 1000.0f);
    }
    else
    {
        shader.setVec3("lightPosition", scene.getLight().getPosition());
    }

    // bind textures
    if (material.diffuseTexture)
    {
        if (!material.diffuseTexture->isValid())
        {
            initializeTexture(*material.diffuseTexture);
        }
        
        bindTexture(*material.diffuseTexture, 0);
        shader.setInt("textureDiffuse", 0);
    }
     
    if (material.normalMap)
    {
        if (!material.normalMap->isValid())
        {
            initializeTexture(*material.normalMap);
        }

        bindTexture(*material.normalMap, 1);
        shader.setBool("useNormalMap", true);
        shader.setInt("textureNormal", 1);
    }

    // Bind shadow map
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap.getDepthTexture());
    shader.setInt("shadowMap", 2);

    mesh.draw();
}

void Renderer::toggleWireframe() {
    static bool wireframe = false;
    wireframe = !wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}

// Static callback for OpenGL debug messages
void Renderer::debugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    // Filter out non-significant error/warning codes
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    std::cerr << "OpenGL Debug Message:" << std::endl;
    std::cerr << "Source: ";
    switch (source) {
        case GL_DEBUG_SOURCE_API: std::cerr << "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cerr << "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cerr << "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cerr << "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: std::cerr << "Other"; break;
    }
    std::cerr << std::endl;

    std::cerr << "Type: ";
    switch (type) {
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
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: std::cerr << "High"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "Medium"; break;
        case GL_DEBUG_SEVERITY_LOW: std::cerr << "Low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Notification"; break;
    }
    std::cerr << std::endl;

    std::cerr << "Message: " << message << std::endl << std::endl;

    // Break on high-severity errors
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        #ifdef _MSC_VER
            __debugbreak();
        #endif
    }
}
