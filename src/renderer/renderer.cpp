#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "core/utils.h"
#include "core/window.h"
#include "texture.h"

bool Renderer::initialize()
{
    if (!initializeOpenGL()) 
    {
        return false;
    }
    
    if (!m_depthShader.init(DEPTH_VERTEX_SHADER, DEPTH_FRAGMENT_SHADER))
    {
        std::cerr << "Failed to initialize depth shader" << std::endl;
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
    if (glewInit() != GLEW_OK) 
    {
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
    if (GLEW_ARB_multisample) 
    {
        glEnable(GL_MULTISAMPLE);
    }

    return true;
}

void Renderer::toggleWireframe() 
{
    static bool wireframe = false;
    wireframe = !wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}

void Renderer::render(GLFWwindow* window, Scene& scene) 
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera setup
    entt::entity activeCamera = scene.getActiveCamera();
    auto* camera = scene.getRegistry().try_get<CameraComponent>(activeCamera);
    auto* cameraTransform = scene.getRegistry().try_get<TransformComponent>(activeCamera);

    if (!camera || !cameraTransform) return;

    // Calculate camera matrices
    glm::mat4 projection = glm::perspective(
        glm::radians(camera->fov),
        float(width) / float(height),
        camera->nearClip,
        camera->farClip
    );
    glm::mat4 view = glm::lookAt(
        cameraTransform->position, 
        cameraTransform->position + forward(*cameraTransform), 
        up(*cameraTransform)
    );

    // First pass: render shadow map
    renderShadowMap(scene);

    // Set viewport
    glViewport(0, 0, width, height);

    // Second pass: render scene
    renderScene(scene, view, projection);
}

void Renderer::renderShadowMap(Scene& scene)
{
    entt::registry& registry = scene.getRegistry();
    auto lightView = registry.view<LightComponent>();

    for(auto [entity, light] : lightView.each())
    {
        if (light.type != LightType::DIRECTIONAL)
        {
            continue;
        }

        m_shadowMap.bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection = glm::ortho(
                -m_shadowOrthoSize, m_shadowOrthoSize, 
                -m_shadowOrthoSize, m_shadowOrthoSize, 
                0.1f, m_shadowDistance * 2
        );
        glm::vec3 lightPos = -light.direction * m_shadowDistance;
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_lightSpaceMatrix = lightProjection * lightView;

        m_depthShader.use();
        m_depthShader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);

        // Render all shadow-casting meshes
        entt::registry& registry = scene.getRegistry();
        auto meshView = registry.view<MeshRendererComponent, TransformComponent>();

        for(auto [entity, meshRenderer, transform] : meshView.each())
        {
            if (!meshRenderer.castShadows)
            {
                continue;
            }

            m_depthShader.setMat4("model", getModelMatrix(transform));
            meshRenderer.mesh->draw();
        }

        m_shadowMap.unbind();

        // TODO: render all shadow-casting lights
        return;
    }
}

void Renderer::renderScene(Scene& scene, const glm::mat4& view, const glm::mat4& projection)
{
    entt::registry& registry = scene.getRegistry();
    auto meshView = registry.view<MeshRendererComponent, TransformComponent>();

    for(auto [entity, meshRenderer, transform] : meshView.each())
    {
        if (!meshRenderer.material)
        {
            continue;
        }

        renderMesh(scene, meshRenderer, transform, view, projection);
    }
}

void Renderer::renderMesh(
    Scene& scene, 
    const MeshRendererComponent& meshRenderer, 
    const TransformComponent& transform,
    const glm::mat4& view, 
    const glm::mat4& projection
) {
    const Material& material = *meshRenderer.material;
    const Shader& shader = *material.shader;
    shader.use();

    // Transform matrices`
    glm::mat4 model = getModelMatrix(transform);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(view * model)));

    shader.setMat4("viewMatrix", view);
    shader.setMat4("modelMatrix", model);
    shader.setMat4("modelViewProjection", projection * view * model);
    shader.setMat3("normalMatrix", normalMatrix);

    // Material textures
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
        shader.setInt("textureNormal", 1);
        shader.setBool("useNormalMap", true);
    }

    // Shadow mapping
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap.getDepthTexture());
    shader.setInt("shadowMap", 2);
    shader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);

    // Set light uniforms for the first active light
    entt::registry& registry = scene.getRegistry();
    auto lightView = registry.view<LightComponent>();;
    for(auto [entity, light] : lightView.each())
    {
        shader.setVec3("lightColor", light.color);
        shader.setFloat("lightPower", light.power);
        shader.setBool("isDirectionalLight", light.type == LightType::DIRECTIONAL);
        
        if (light.type == LightType::DIRECTIONAL) 
        {
            shader.setVec3("lightPosition", -light.direction * 1000.0f);
        } 
        else 
        {
            shader.setVec3("lightPosition", light.position);
        }

        // TODO: Extend for multiple lights
        break;
    }

    meshRenderer.mesh->draw();
}

// Static callback for OpenGL debug messages
void Renderer::debugCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
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
