#include "application.h"

#include <iostream>
#include <algorithm>

#include "uuid.h"
#include "utils.h"
#include "input.h"
#include "assert.h"
#include "scene/components.h"

namespace Engine {

bool Application::initialize(int width, int height, const char* title)
{
    UUID_init();

    m_sdk.window = std::make_unique<Window>(width, height, title);
    m_sdk.renderer = std::make_unique<OpenGL::Renderer>();
    m_sdk.scene = std::make_unique<Scene>();
    m_sdk.uiManager = std::make_unique<UIManager>();
    m_sdk.resourceManager = std::make_unique<ResourceManager>();
    
    if (!m_sdk.renderer->initialize()) return false;
    if (!m_sdk.uiManager->initialize(*m_sdk.window)) return false;
    Input::init(*m_sdk.window);
    
    m_fpsCameraSystem = std::make_unique<Editor::FPSCameraSystem>();
    m_editorUI = std::make_unique<Editor::EditorUI>();

    return true;
}

bool Application::loadScene(const std::string& path)
{
    return m_sdk.scene->loadScene(path, *m_sdk.resourceManager);
}

void Application::run() 
{   
    while (!m_sdk.window->shouldClose())
    {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        m_sdk.window->pollEvents();
        m_sdk.uiManager->startFrame();
        
        update(deltaTime);
        render();

        m_sdk.uiManager->endFrame();
        m_sdk.window->swapBuffers();
    }
}

void Application::update(float deltaTime) 
{
    m_fpsCameraSystem->update(*m_sdk.scene, deltaTime, m_editorUI->isSceneViewActive());
}

void Application::render() 
{
    m_sdk.renderer->render(m_editorUI->getFramebufferSize(), *m_sdk.scene);
    
    m_editorUI->setupDockingSpace();
    m_editorUI->renderMenuBar(m_sdk);
    m_editorUI->renderSceneView(static_cast<uintptr_t>(m_sdk.renderer->getFrameBuffer().colorTexture));
    m_editorUI->renderEntityBrowser(*m_sdk.scene);
    m_editorUI->renderEntityDetails(*m_sdk.scene);
}

void Application::cleanup()
{
    m_sdk.uiManager->cleanup();
    m_sdk.renderer->cleanup();
    m_sdk.resourceManager->cleanup();
    m_sdk.window.reset();
}

}