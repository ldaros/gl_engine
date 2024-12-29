#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void drawPyramid() {
    glBegin(GL_TRIANGLES);

    // Base (2 triangles)
    glColor3f(1.0f, 0.0f, 0.0f);  // Red
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);

    glColor3f(0.0f, 1.0f, 0.0f);  // Green
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);

    // Sides
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.0f, 0.8f, 0.0f);

    glColor3f(0.0f, 1.0f, 1.0f);  // Cyan
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.0f, 0.8f, 0.0f);

    glColor3f(1.0f, 0.0f, 1.0f);  // Magenta
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.0f, 0.8f, 0.0f);

    glColor3f(0.5f, 0.5f, 0.5f);  // Gray
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.0f, 0.8f, 0.0f);

    glEnd();
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1200, 900, "Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main loop
    float angle = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);

        // Set up the model-view matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0.0, 1.5, 3.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0);

        // Rotate the pyramid
        glPushMatrix();
        glTranslatef(0.0f, 0.5f, 0.0f);  // Center the pyramid
        glRotatef(angle, 0.0f, 1.0f, 0.0f);  // Rotate around the Y-axis
        drawPyramid();
        glPopMatrix();

        // Increment the rotation angle
        angle += 1.0f;
        if (angle >= 360.0f) angle -= 360.0f;

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}