#include "application.h"

#include <iostream>
#include <algorithm>

#include "uuid.h"
#include "utils.h"
#include "input.h"
#include "assert.h"
#include "scene/components.h"

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

    m_resourceManager = std::make_unique<ResourceManager>();

    Input::init(*m_window);

    return true;
}

bool Application::loadScene(const std::string& path)
{
    return m_scene->loadScene(path, *m_resourceManager);
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
    m_fpsCameraSystem->update(*m_scene, deltaTime, m_editorUI->isSceneViewActive());
}

void Application::render() 
{
    m_renderer->render(m_editorUI->getFramebufferSize(), *m_scene);
    
    m_editorUI->setupDockingSpace();
    m_editorUI->renderMenuBar();
    m_editorUI->renderSceneView(static_cast<uintptr_t>(m_renderer->getFrameBuffer().colorTexture));
    m_editorUI->renderEntityBrowser(*m_scene);
    m_editorUI->renderEntityDetails(*m_scene);
}

void Application::cleanup()
{
    m_uiManager->cleanup();
    m_renderer->cleanup();
    m_resourceManager->cleanup();
    m_window.reset();
}

}