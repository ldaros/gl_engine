#include "application.h"

#include <iostream>
#include <algorithm>

#include "uuid.h"
#include "utils.h"
#include "input.h"
#include "assert.h"
#include "scene/components.h"
#include "resource_manager.h"

namespace Engine {

Application::Application() {}

Application::~Application() {}

bool Application::initialize(int width, int height, const char* title)
{
    UUID_init();

    m_window = std::make_unique<Window>(width, height, title);
    if (!m_window) return false;

    m_renderer = std::make_unique<OpenGL::Renderer>();
    if (!m_renderer->initialize()) return false;

    m_scene = std::make_unique<Scene>();

    m_uiManager = std::make_unique<UIManager>();
    m_uiManager->initialize(*m_window);

    m_fpsCameraSystem = std::make_unique<Editor::FPSCameraSystem>();
    m_editorUI = std::make_unique<Editor::EditorUI>();

    Input::init(*m_window);

    ResourceManager& resourceManager = ResourceManager::getInstance();

    std::shared_ptr<MeshData> shadowTestMesh = resourceManager.loadMesh("resources/assets/shadow_test.fbx");
    ASSERT(shadowTestMesh, "Failed to load mesh");

    std::shared_ptr<MeshData> teapotMesh = resourceManager.loadMesh("resources/assets/teapot.fbx");
    ASSERT(teapotMesh, "Failed to load mesh");

    auto albedoTexture = resourceManager.loadTexture("resources/textures/default.png");
    ASSERT(albedoTexture, "Failed to load albedo texture");

    auto normalMap = resourceManager.loadTexture("resources/textures/normal.png");
    ASSERT(normalMap, "Failed to load normal map");

    auto material = std::make_shared<Material>();
    material->albedo = albedoTexture;
    // material->normal = normalMap;

    // create entities
    entt::registry& registry = m_scene->getRegistry();

    entt::entity meshEntity = registry.create();
    registry.emplace<NameComponent>(meshEntity, "ShadowTestMesh");
    registry.emplace<MeshRendererComponent>(
        meshEntity,
        MeshRendererComponent{
            .meshData = shadowTestMesh,
            .material = material,
        }
    );
    registry.emplace<TransformComponent>(
        meshEntity,
        TransformComponent{
            .rotation = glm::vec3(0.0f, 0.0f, 0.0f),
            .position = glm::vec3(0.0f, 0.0f, 0.0f),
            .scale = glm::vec3(0.1f)
        }
    );

    entt::entity meshEntity2 = registry.create();
    registry.emplace<NameComponent>(meshEntity2, "TeapotMesh");
    registry.emplace<MeshRendererComponent>(
        meshEntity2,
        MeshRendererComponent{
            .meshData = teapotMesh,
            .material = material,
        }
    );
    registry.emplace<TransformComponent>(
        meshEntity2,
        TransformComponent{
            .rotation = glm::vec3(0.0f),
            .position = glm::vec3(0.2f, 0.2f, 0.5f),
            .scale = glm::vec3(0.05f)
        }
    );

    entt::entity cameraEntity = registry.create();
    registry.emplace<NameComponent>(cameraEntity, "MainCamera");
    registry.emplace<CameraComponent>(
        cameraEntity,
        CameraComponent{
            .fov = 45.0f,
            .nearClip = 0.1f,
            .farClip = 1000.0f,
            .active = true
        }
    );
    registry.emplace<TransformComponent>(
        cameraEntity,
        TransformComponent{
            .rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            .position = glm::vec3(0.0f, 1.0f, 5.0f),
            .scale = glm::vec3(1.0f)
        }
    );

    entt::entity lightEntity = registry.create();
    registry.emplace<NameComponent>(lightEntity, "DirectionalLight");
    registry.emplace<LightComponent>(
        lightEntity,
        LightComponent{
            .position = glm::vec3(0.0f, 3.0f, 5.0f),
            .direction = glm::vec3(0.5f, -0.5f, -1.0f),
            .color = glm::vec3(1.0f),
            .power = 1.0f,
            .type = LightType::DIRECTIONAL
        }
    );

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
        m_uiManager->startFrame();
        
        update(deltaTime);
        render();

        m_uiManager->endFrame();
        m_window->swapBuffers();
    }
}

void Application::update(float deltaTime) 
{
    m_fpsCameraSystem->update(deltaTime, *m_scene);
}

void Application::render() 
{
    m_renderer->render(m_editorUI->getFramebufferSize(), *m_scene);
    
    m_editorUI->setupDockingSpace();
    m_editorUI->renderSceneView(static_cast<uintptr_t>(m_renderer->getFrameBuffer().colorTexture));
    m_editorUI->renderEntityBrowser(*m_scene);
    m_editorUI->renderEntityDetails(*m_scene);
}

void Application::cleanup()
{
    m_uiManager->cleanup();
    m_renderer->cleanup();
    ResourceManager::getInstance().cleanup();
    m_window.reset();
}

}