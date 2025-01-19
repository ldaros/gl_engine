#include "application.h"

#include "input.h"

Application::Application() {}

Application::~Application() {}

bool Application::initialize(int width, int height, const char* title)
{
    m_window = std::make_unique<Window>(width, height, title);
    if (!m_window) return false;

    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->initialize()) return false;

    m_scene = std::make_unique<Scene>();
    if (!m_scene->initialize()) return false;

    m_uiManager = std::make_unique<UIManager>();
    m_uiManager->initialize(m_window->getHandle());

    Input::init(m_window->getHandle());
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

        int width, height;
        m_window->getFramebufferSize(width, height);
        m_renderer->setupFrame(width, height);
        m_uiManager->newFrame();
        
        update(deltaTime);
        render();

        m_window->swapBuffers();
    }
}

void Application::update(float deltaTime) 
{
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

    // Toggle light controls
    if (Input::isKeyDown(Key::L) && !m_lightTogglePressed) 
    {
        m_uiManager->toggleLightingWindow();
        m_lightTogglePressed = true;
    } 
    else if (Input::isKeyReleased(Key::L)) 
    {
        m_lightTogglePressed = false;
    }

    // Toggle performance window
    if (Input::isKeyDown(Key::P) && !m_performanceTogglePressed) 
    {
        m_uiManager->togglePerformanceWindow();
        m_performanceTogglePressed = true;
    } 
    else if (Input::isKeyReleased(Key::P)) 
    {
        m_performanceTogglePressed = false;
    }

    // Toggle transform window
    if (Input::isKeyDown(Key::T) && !m_transformTogglePressed) 
    {
        m_uiManager->toggleTransformWindow();
        m_transformTogglePressed = true;
    } 
    else if (Input::isKeyReleased(Key::T)) 
    {
        m_transformTogglePressed = false;
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
        auto& camera = m_scene->getCamera();

        // Handle keyboard inputs to move the camera
        if (Input::isKeyDown(Key::W)) 
        {
            camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
        }
        if (Input::isKeyDown(Key::S))
        {
            camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
        }
        if (Input::isKeyDown(Key::D))
        {
            camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
        }
        if (Input::isKeyDown(Key::A))
        {
            camera.processKeyboard(CameraMovement::LEFT, deltaTime);
        }
        if (Input::isKeyDown(Key::SPACE))
        {
            camera.processKeyboard(CameraMovement::UP, deltaTime);
        }
        if (Input::isKeyDown(Key::LEFT_SHIFT))
        {
            camera.processKeyboard(CameraMovement::DOWN, deltaTime);
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
        camera.processMouseMovement(xOffset, yOffset);
    }
}

void Application::render() 
{
    m_renderer->render(m_window->getHandle(), *m_scene, m_scene->getCamera());
    m_uiManager->render(*m_scene);
}

void Application::cleanup()
{
    m_scene->cleanup();
    m_uiManager->cleanup();
    m_window.reset();
}
