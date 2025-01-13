#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "core/input.h"
#include "core/file_system.h"
#include "core/obj_loader.h"
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

    // Set OpenGL version and profile (4.1 core profile)
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL 4.1
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Enable forward compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile

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

    // Initialize input
    Input::init(window);

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
    // Culling is diabled right now due to obj loader not working properly
    // glEnable(GL_CULL_FACE); // Enable face culling

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
    
    // Camera setup
    Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    float lastMouseX, lastMouseY;
    bool firstMouse = true;
    bool isRightMouseButtonPressed = false;

    // Wireframe mode
    bool wireframeTogglePressed = false;
    bool wireframe = false;

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrameTime = 0.0f; // Time of the last frame

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Get the current time
        float currentFrameTime = static_cast<float>(glfwGetTime());
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Resize the viewport
        int screenWidth, screenHeight;
        glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
        glViewport(0, 0, screenWidth, screenHeight);
        
        // Pool events
        glfwPollEvents();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark gray background

        // Toggle wireframe mode
        if (Input::isKeyDown(Key::F) && !wireframeTogglePressed) {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            wireframeTogglePressed = true;
        } else if (Input::isKeyReleased(Key::F)) {
            wireframeTogglePressed = false;
        }

        // Check if the right mouse button is pressed or released
        bool rightMouseButtonCurrentlyPressed = Input::isMouseDown(Mouse::BUTTON_RIGHT);
        if (rightMouseButtonCurrentlyPressed && !isRightMouseButtonPressed)
        {
            // Right mouse button pressed - enable camera movement and hide cursor
            Input::disableCursor();
            isRightMouseButtonPressed = true;
            firstMouse = true; // Reset mouse handling
        }
        else if (!rightMouseButtonCurrentlyPressed && isRightMouseButtonPressed)
        {
            // Right mouse button released - disable camera movement and show cursor
            Input::enableCursor();
            isRightMouseButtonPressed = false;
        }

        // If the right mouse button is pressed, handle camera movement
        if (isRightMouseButtonPressed)
        {
            // Handle keyboard inputs to move the camera
            if (Input::isKeyDown(Key::W)) {
                camera.processKeyboard(FORWARD, deltaTime);
            }
            if (Input::isKeyDown(Key::S)) {
                camera.processKeyboard(BACKWARD, deltaTime);
            }
            if (Input::isKeyDown(Key::D)) {
                camera.processKeyboard(RIGHT, deltaTime);
            }
            if (Input::isKeyDown(Key::A)) {
                camera.processKeyboard(LEFT, deltaTime);
            }

            // Handle mouse movement
            auto mousePosition = Input::getMousePosition();
            if (firstMouse) {
                lastMouseX = mousePosition.x;
                lastMouseY = mousePosition.y;
                firstMouse = false;
            }
            float xOffset = mousePosition.x - lastMouseX;
            float yOffset = lastMouseY - mousePosition.y;
            lastMouseX = mousePosition.x;
            lastMouseY = mousePosition.y;
            camera.processMouseMovement(xOffset, yOffset);
        }

        // Setup matrices
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), 
            float(screenWidth) / float(screenHeight),
            0.1f, 
            100.0f
        );
        glm::mat4 view = camera.getViewMatrix();

        // Set shader uniforms
        shader.setMat4("model", glm::mat4(1.0f));
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
            // set position of the window
            ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f));
            ImGui::SetNextWindowBgAlpha(0.5f); // Set background transparency
            ImGui::Begin(
                "Overlay", 
                &showGui, 
                ImGuiWindowFlags_AlwaysAutoResize | 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove
            );
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Camera position : %f, %f, %f", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("Camera direction : %f, %f, %f", camera.Front.x, camera.Front.y, camera.Front.z);
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