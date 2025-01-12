#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "core/file_system.h"
#include "core/obj_loader.h"
#include "core/transform.h"
#include "renderer/shader.h"
#include "renderer/camera.h"
#include "renderer/mesh.h"
#include "renderer/texture.h"

#define GLCheckError() \
    { GLenum err; while((err = glGetError()) != GL_NO_ERROR) \
    std::cerr << "OpenGL error: " << err << " at line " << __LINE__ << std::endl; }

void glfwErrorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void openGLDebugCallback(
    GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar* message,
    const void* userParam
)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        std::cerr << "OpenGL Debug: " << message << std::endl;
    }
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL 4.1
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    GLCheckError();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    bool showGui = true;
    GLCheckError();

    glEnable(GL_DEBUG_OUTPUT); // Enable debug output for OpenGL errors
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Enable synchronous debug output
    glDebugMessageCallback(openGLDebugCallback, nullptr); // Set the debug callback
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    glDepthFunc(GL_LESS);    // Depth test passes when the new depth value is less than the stored value

    // Initialize shader
    Shader shader;
    shader.init(
        FileSystem::read("../resources/vertex_shader.glsl"),  // Vertex shader source
        FileSystem::read("../resources/fragment_shader.glsl") // Fragment shader source
    );
    GLuint shaderProgram = shader.m_id;
    glUseProgram(shaderProgram);
    GLCheckError();

    // Load OBJ file
    std::vector<float> vertices; // 3 floats per vertex (x, y, z)
    std::vector<float> texCoords; // 2 floats per vertex (u, v)
    if (!ObjLoader::loadOBJ("../resources/pyramid.obj", vertices, texCoords))
    {
        std::cerr << "Failed to load OBJ file" << std::endl;
        return -1;
    }
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Texture coordinates: " << texCoords.size() << std::endl;

    // Create mesh
    Mesh mesh(vertices, texCoords);

    // Load texture
    Texture texture("../resources/texture.DDS");
    GLuint textureID = texture.getID();
    
    // Camera and Transform
    Camera camera(glm::vec3(0.f, 0.f, 2.f), glm::vec3(0.f, 0.f, -1.f));
    Transform transform;

    // Calculate the center of the mesh
    glm::vec3 meshCenter = transform.calculateCenter(vertices);
    
    // Get screen dimensions
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Pool events
        glfwPollEvents();

        float time = (float)glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark gray background

        // Setup matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(glm::radians(45.0f), screenWidth / (float)screenHeight, 0.1f, 100.f);

        // Reset the transform and apply rotation around the pyramid's center
        transform.reset();
        transform.translate(-meshCenter); // Move to origin
        transform.rotate(time, glm::vec3(0.f, 1.f, 0.f)); // Rotate around the Y-axis
        transform.translate(meshCenter); // Move back to original position

        // Use the shader
        shader.setMat4("model", transform.getModelMatrix());
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // Draw mesh
        mesh.draw(shaderProgram, textureID);
        GLCheckError();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show GUI
        if (showGui)
        {
            ImGui::Begin("Controls", &showGui);
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            auto cameraPosition = camera.getPosition();
            ImGui::SliderFloat3("Camera Position", &cameraPosition.x, -5.0f, 5.0f);
            camera.setPosition(cameraPosition);
            auto cameraDirection = camera.getDirection();
            ImGui::SliderFloat3("Camera Direction", &cameraDirection.x, -5.0f, 5.0f);
            camera.setDirection(cameraDirection);
            ImGui::End();
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }
    
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}   