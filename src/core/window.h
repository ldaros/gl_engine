#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window 
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool shouldClose();
    void swapBuffers();
    void pollEvents();
    GLFWwindow* getHandle();
    void getFramebufferSize(int& width, int& height);

private:
    GLFWwindow* m_window;
    static void errorCallback(int error, const char* description);
};


#endif // WINDOW_H