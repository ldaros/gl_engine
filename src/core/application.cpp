#include "application.h"

#include <algorithm>
#include "input.h"
#include "scene/components.h"
#include "resource_manager.h"
#include "core/utils.h"

Application::Application() {}

Application::~Application() {}

bool Application::initialize(int width, int height, const char* title)
{
    m_window = std::make_unique<Window>(width, height, title);
    if (!m_window) return false;

    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->initialize()) return false;

    m_scene = std::make_unique<Scene>();

    m_uiManager = std::make_unique<UIManager>();
    m_uiManager->initialize(m_window->getHandle());

    Input::init(m_window->getHandle());

    ResourceManager& resourceManager = ResourceManager::getInstance();

    std::shared_ptr<MeshData> meshData = resourceManager.loadMesh("resources/assets/shadow_test.fbx");
    if (!meshData)
    {
        std::cerr << "Failed to load mesh" << std::endl;
        return false;
    }
    m_mesh = std::make_shared<Mesh>();
    if (!m_mesh->initialize(*meshData))
    {
        std::cerr << "Failed to initialize mesh" << std::endl;
        return false;
    }

    std::shared_ptr<MeshData> meshData2 = resourceManager.loadMesh("resources/assets/teapot.fbx");
    if (!meshData2)
    {
        std::cerr << "Failed to load mesh" << std::endl;
        return false;
    }
    m_mesh2 = std::make_shared<Mesh>();
    if (!m_mesh2->initialize(*meshData2))
    {
        std::cerr << "Failed to initialize mesh" << std::endl;
        return false;
    }

    std::shared_ptr<ShaderData> shaderData = resourceManager.loadShader(
        "resources/shaders/standard_vs.glsl",
        "resources/shaders/standard_fs.glsl"
    );
    if (!shaderData)
    {
        std::cerr << "Failed to load shader" << std::endl;
        return false;
    }
    m_shader = std::make_shared<Shader>();
    if(!m_shader->init(shaderData->vertexCode, shaderData->fragmentCode))
    {
        std::cerr << "Failed to compile shader" << std::endl;
        return false;
    }

    m_diffuseTexture = resourceManager.loadTexture("resources/textures/default.png");
    if (!m_diffuseTexture)
    {
        std::cerr << "Failed to load diffuse texture" << std::endl;
        return false;
    }
    m_normalMap = resourceManager.loadTexture("resources/textures/normal.png");
    if (!m_normalMap)
    {
        std::cerr << "Failed to load normal map texture" << std::endl;
    }

    m_material = std::make_shared<Material>();
    m_material->shader = m_shader;
    m_material->diffuseTexture = m_diffuseTexture;
    m_material->normalMap = m_normalMap;

    // create entities
    entt::registry& registry = m_scene->getRegistry();

    entt::entity meshEntity = registry.create();
    registry.emplace<NameComponent>(meshEntity, "ShadowTestMesh");
    registry.emplace<MeshRendererComponent>(
        meshEntity,
        m_mesh, 
        m_material, 
        true
    );
    registry.emplace<TransformComponent>(
        meshEntity,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.1f)
    );
    registry.emplace<Renderable>(meshEntity);

    entt::entity meshEntity2 = registry.create();
    registry.emplace<NameComponent>(meshEntity2, "TeapotMesh");
    registry.emplace<MeshRendererComponent>(
        meshEntity2,
        m_mesh2,
        m_material,
        true
    );
    registry.emplace<TransformComponent>(
        meshEntity2,
        glm::vec3(0.0f),
        glm::vec3(0.2f, 0.2f, 0.5f),
        glm::vec3(0.05f)
    );

    entt::entity cameraEntity = registry.create();
    registry.emplace<NameComponent>(cameraEntity, "MainCamera");
    registry.emplace<CameraComponent>(
        cameraEntity,
        45.0f,   // fov
        0.1f,    // nearClip
        1000.0f, // farClip
        true     // active
    );
    registry.emplace<TransformComponent>(
        cameraEntity,
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f), // rotation
        glm::vec3(0.0f, 1.0f, 5.0f),  // position
        glm::vec3(1.0f) // scale
    );
    registry.emplace<PerspectiveCamera>(cameraEntity);

    entt::entity lightEntity = registry.create();
    registry.emplace<NameComponent>(lightEntity, "DirectionalLight");
    registry.emplace<LightComponent>(
        lightEntity,
        glm::vec3(0.0f, 3.0f, 5.0f),  // position
        glm::vec3(0.5f, -0.5f, -1.0f), // direction
        glm::vec3(1.0f),              // color
        1.0f,                         // intensity
        LightType::DIRECTIONAL
    );
    registry.emplace<LightSource>(lightEntity);

    return true;
}

void Application::run() 
{   
    while (!m_window->shouldClose()) 
    {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        m_window->pollEvents();
        m_uiManager->newFrame();
        
        update(deltaTime);
        render();

        m_window->swapBuffers();
    }
}

void Application::update(float deltaTime) 
{
    entt::registry& registry = m_scene->getRegistry();

    // Toggle wireframe mode
    if (Input::isKeyDown(Key::F) && !m_wireframeTogglePressed)
    {
        m_renderer->toggleWireframe();
        m_wireframeTogglePressed = true;
    } 
    else if (Input::isKeyReleased(Key::F)) 
    {
        m_wireframeTogglePressed = false;
    }

    bool rightMouseButtonCurrentlyPressed = Input::isMouseDown(Mouse::BUTTON_RIGHT);

    if (rightMouseButtonCurrentlyPressed && !m_isRightMouseButtonPressed)
    {
        Input::disableCursor();
        m_isRightMouseButtonPressed = true;
        m_firstMouse = true;
    }
    else if (!rightMouseButtonCurrentlyPressed && m_isRightMouseButtonPressed)
    {
        Input::enableCursor();
        m_isRightMouseButtonPressed = false;
    }

    if (m_isRightMouseButtonPressed)
    {
        auto cameraEntity = m_scene->findEntityByName("MainCamera");

        if (cameraEntity == entt::null)
        {
            std::cerr << "MainCamera entity not found!" << std::endl;
            return;
        }

        auto* camera = registry.try_get<CameraComponent>(cameraEntity);
        auto* cameraTransform = registry.try_get<TransformComponent>(cameraEntity);

        // Handle keyboard inputs to move the camera
        float velocity = m_movementSpeed * deltaTime;
        glm::vec3 position = cameraTransform->position;

        if (Input::isKeyDown(Key::W)) 
        {
            cameraTransform->position += forward(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::S))
        {
            cameraTransform->position -= forward(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::D))
        {
            cameraTransform->position += right(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::A))
        {
            cameraTransform->position -= right(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::SPACE))
        {
            cameraTransform->position += up(*cameraTransform) * velocity;
        }
        if (Input::isKeyDown(Key::LEFT_SHIFT))
        {
            cameraTransform->position -= up(*cameraTransform) * velocity;
        }

        // Handle mouse movement
        auto mousePosition = Input::getMousePosition();
        if (m_firstMouse) 
        {
            m_lastMouseX = mousePosition.x;
            m_lastMouseY = mousePosition.y;
            m_firstMouse = false;
        }
        float xOffset = m_lastMouseX - mousePosition.x;
        float yOffset = m_lastMouseY - mousePosition.y;
        m_lastMouseX = mousePosition.x;
        m_lastMouseY = mousePosition.y;

        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        // Update rotation angles
        m_rotationAngles.x += xOffset; // Horizontal rotation
        m_rotationAngles.y += yOffset; // Vertical rotation
    
        // Constrain the vertical rotation
        m_rotationAngles.y = std::clamp(m_rotationAngles.y, -89.0f, 89.0f);

        // Create quaternions for horizontal and vertical rotations
        glm::quat horizontalQuat = glm::angleAxis(
            glm::radians(m_rotationAngles.x),
            glm::vec3(0.0f, 1.0f, 0.0f)  // Rotate around Y axis
        );
        
        glm::quat verticalQuat = glm::angleAxis(
            glm::radians(m_rotationAngles.y),
            glm::vec3(1.0f, 0.0f, 0.0f)  // Rotate around X axis
        );

        // Combine rotations and set the transform's rotation
        cameraTransform->rotation = horizontalQuat * verticalQuat;
    }
}

void Application::render() 
{
    m_renderer->render(m_window->getHandle(), *m_scene);
    m_uiManager->render(*m_scene);
}

void Application::cleanup()
{
    m_uiManager->cleanup();
    m_window.reset();
}
