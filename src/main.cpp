#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "file_manager/file_manager.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "transform/transform.h"
#include "mesh/mesh.h"

void glfwErrorCallback(int error, const char *description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main() {
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL 4.1
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *window = glfwCreateWindow(1200, 900, "OG Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Initialize shader
    Shader shader;
    shader.init(
        FileManager::read("../src/shaders/vertex_shader.glsl"),  // Vertex shader source
        FileManager::read("../src/shaders/fragment_shader.glsl") // Fragment shader source
    );

   // Create triangle
   std::vector<float> vertices = {
        0.0f, 1.0f, 0.0f,  // Position of vertex 1
        1.0f, 0.0f, 0.0f,  // Color of vertex 1 (Red)
        -1.f, -0.5f, 0.0f, // Position of vertex 2
        0.0f, 1.0f, 0.0f,  // Color of vertex 2 (Green)
        1.0f, -0.5f, 0.0f, // Position of vertex 3
        0.0f, 0.0f, 1.0f   // Color of vertex 3 (Blue)
    };
    std::vector<unsigned int> indices = {
        0, 1, 2 // The triangle consists of vertices 0, 1, and 2
    };
    Mesh triangle(vertices, indices);

    // Camera and Transform
    Camera camera(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f, 0.f, -1.f));
    Transform transform;

    // Get screen dimensions
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(glm::radians(45.0f), screenWidth / (float)screenHeight, 0.1f, 100.f);

        // Apply rotation transformation
        transform.rotate(glm::sin(time * 0.8f) / 4.f, glm::vec3(0.f, 0.f, -1.f));

        shader.use();
        shader.setMat4("u_model", transform.getModelMatrix());
        shader.setMat4("u_view", view);
        shader.setMat4("u_projection", projection);

        // Draw mesh
        triangle.draw();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}