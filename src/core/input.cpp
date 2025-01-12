#include "input.h"

GLFWwindow* Input::m_window = nullptr;

void Input::init(GLFWwindow* window)
{
    m_window = window;
}

bool Input::isKeyDown(Key::KeyCode key)
{
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Input::isKeyReleased(Key::KeyCode key)
{
    return glfwGetKey(m_window, key) == GLFW_RELEASE;
}

bool Input::isMouseDown(Mouse::MouseCode button)
{
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

bool Input::isMouseReleased(Mouse::MouseCode button)
{
    return glfwGetMouseButton(m_window, button) == GLFW_RELEASE;
}

MousePosition Input::getMousePosition()
{
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    return { xpos, ypos };
}

void Input::disableCursor()
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::enableCursor()
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
