#include "input.h"

namespace Engine {

GLFWwindow* Input::m_window = nullptr;

void Input::init(Window& window)
{
    m_window = window.getHandle();
}

bool Input::isKeyDown(Key key)
{
    ASSERT(m_window != nullptr, "Window not initialized");
    return glfwGetKey(m_window, static_cast<int>(key)) == GLFW_PRESS;
}

bool Input::isKeyReleased(Key key)
{   
    ASSERT(m_window != nullptr, "Window not initialized");
    return glfwGetKey(m_window, static_cast<int>(key)) == GLFW_RELEASE;
}

bool Input::isMouseDown(Mouse button)
{
    ASSERT(m_window != nullptr, "Window not initialized");
    return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_PRESS;
}

bool Input::isMouseReleased(Mouse button)
{
    ASSERT(m_window != nullptr, "Window not initialized");
    return glfwGetMouseButton(m_window, static_cast<int>(button)) == GLFW_RELEASE;
}

MousePosition Input::getMousePosition()
{
    ASSERT(m_window != nullptr, "Window not initialized");

    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    return { xpos, ypos };
}

void Input::disableCursor()
{
    ASSERT(m_window != nullptr, "Window not initialized");
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::enableCursor()
{
    ASSERT(m_window != nullptr, "Window not initialized");
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

}