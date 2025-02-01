#pragma once

#include <utility>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Engine {

class Window 
{
public:
    Window(uint32_t width, uint32_t height, const std::string& title);
    ~Window();

    bool shouldClose() const;
    void swapBuffers() const;
    void pollEvents() const;
    
    GLFWwindow* getHandle() const;
    std::pair<uint32_t, uint32_t> getFramebufferSize() const;

private:
    GLFWwindow* m_window;
    static void errorCallback(int error, const char* description);
};

}