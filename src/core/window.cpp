#include "window.h"

#include <iostream>

namespace Engine {

Window::Window(uint32_t width, uint32_t height, const std::string& title)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);

    // Set OpenGL version and profile (4.5 core profile)
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // OpenGL 4.5
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Enable forward compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile

    // Create window
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

GLFWwindow* Window::getHandle() const
{
    return m_window;
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

std::pair<uint32_t, uint32_t> Window::getFramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void Window::pollEvents() const
{
    glfwPollEvents();
}

void Window::close()
{
    glfwSetWindowShouldClose(m_window, true);
}

void Window::errorCallback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

}
