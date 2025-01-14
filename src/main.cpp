#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "core/input.h"
#include "core/file_system.h"
#include "core/obj_loader.h"
#include "core/transform.h"
#include "core/utils.h"
#include "renderer/shader.h"
#include "renderer/camera.h"
#include "renderer/mesh.h"
#include "renderer/texture.h"
#include "renderer/light.h"

#define GLCheckError() \
    { GLenum err; while((err = glGetError()) != GL_NO_ERROR) \
    std::cerr << "OpenGL error: " << err << " at line " << __LINE__ << std::endl; }

void glfwErrorCallback(int error, const char *description);
void openGLDebugCallback(
    GLenum source,
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar* message, 
    const void* userParam
);
void initializeGLFW();
void initializeGLEW();
void initializeImGui(GLFWwindow* window);
void cleanup(GLFWwindow* window);

int main()
{
    initializeGLFW();

    // Create window
    GLFWwindow *window = glfwCreateWindow(1200, 900, "OG Engine", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    Input::init(window);
    initializeGLEW();
    GLCheckError();

    initializeImGui(window);
    GLCheckError();

    glEnable(GL_DEBUG_OUTPUT); // Enable debug output for OpenGL errors
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Enable synchronous debug output
    glDebugMessageCallback(openGLDebugCallback, nullptr); // Set the debug callback
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    glDepthFunc(GL_LESS);    // Depth test passes when the new depth value is less than the stored value
    glEnable(GL_CULL_FACE); // Enable face culling
    glCullFace(GL_BACK); // Cull back face
    glEnable(GL_BLEND); // Enable blending for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function to source alpha and one minus source alpha

    // Initialize shader
    Shader shader;
    if (!shader.init(
            FileSystem::read("../resources/standard_vs.glsl"),
            FileSystem::read("../resources/standard_fs.glsl"))) 
    {
        std::cerr << "Failed to initialize shader" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Load OBJ file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> UVs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    if (!ObjLoader::loadOBJ("../resources/teapot.obj", vertices, UVs, normals, indices))
    {
        std::cerr << "Failed to load OBJ file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Calculate tangents and bitangents
    std::vector<glm::vec3> tangents, bitangents;
    calculateTangents(vertices, UVs, normals, indices, tangents, bitangents);

    // Create mesh
    Mesh mesh(vertices, UVs, normals, indices, tangents, bitangents);
    
    // Load textures
    Texture texture("../resources/default.DDS");
    GLuint textureID = texture.getID();
    Texture normalMap("../resources/normal.DDS");
    GLuint normalMapID = normalMap.getID();

    Transform transform;
    
    // Camera setup
    Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    float lastMouseX, lastMouseY;
    bool firstMouse = true;
    bool isRightMouseButtonPressed = false;

    // Light setup
    Light light;
    light.setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    light.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    light.setPower(20.0f);

    bool wireframeTogglePressed = false;
    bool wireframe = false;
    bool showGui = true;
    bool showLightControls = false;
    bool lightTogglePressed = false;

    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrameTime = 0.0f; // Time of the last frame

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Time management
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

        // Toggle light controls
        if (Input::isKeyDown(Key::L) && !lightTogglePressed) {
            showLightControls = !showLightControls;
            lightTogglePressed = true;
        } else if (Input::isKeyReleased(Key::L)) {
            lightTogglePressed = false;
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
            if (Input::isKeyDown(Key::W)) 
            {
                camera.processKeyboard(FORWARD, deltaTime);
            }
            if (Input::isKeyDown(Key::S))
            {
                camera.processKeyboard(BACKWARD, deltaTime);
            }
            if (Input::isKeyDown(Key::D))
            {
                camera.processKeyboard(RIGHT, deltaTime);
            }
            if (Input::isKeyDown(Key::A))
            {
                camera.processKeyboard(LEFT, deltaTime);
            }

            // Handle mouse movement
            auto mousePosition = Input::getMousePosition();
            if (firstMouse) 
            {
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

        // scale mesh down
        transform.reset();
        transform.scale(glm::vec3(0.1f));

        // Setup matrices
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), 
            float(screenWidth) / float(screenHeight),
            0.1f, 
            100.0f
        );
        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 modelMatrix = transform.getModelMatrix();
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(viewMatrix * modelMatrix)));

        // Set shader uniforms
        shader.use();
        shader.setMat4("modelViewProjection", projection * viewMatrix * modelMatrix);
        shader.setMat4("viewMatrix", viewMatrix);
        shader.setMat4("modelMatrix", modelMatrix);
        shader.setMat3("normalMatrix", normalMatrix);
        shader.setVec3("lightPosition", light.getPosition());
        shader.setFloat("lightPower", light.getPower());
        shader.setVec3("lightColor", light.getColor());
    
        // Draw mesh
        mesh.draw(shader.m_id, textureID, normalMapID);
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
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::Text("Total Vertices: %d", mesh.getVertexCount());
            ImGui::Text("Total Indices: %d", mesh.getIndexCount());
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::Text("Camera position : %f, %f, %f", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("Camera direction : %f, %f, %f", camera.Front.x, camera.Front.y, camera.Front.z);
            ImGui::End();
        }

        if (showLightControls)
        {
            ImGui::SetNextWindowPos(ImVec2(20.0f, 200.0f));
            ImGui::SetNextWindowBgAlpha(0.5f);
            ImGui::Begin("Light Controls", &showLightControls, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            auto color = light.getColor();
            auto power = light.getPower();
            auto position = light.getPosition();
            ImGui::SliderFloat("Power", &power, 0.0f, 100.0f);
            light.setPower(power);
            ImGui::ColorEdit3("Color", &color.x);
            light.setColor(color);
            ImGui::DragFloat3("Position", &position.x);
            light.setPosition(position);
            ImGui::End();
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }
    
    cleanup(window);
    return 0;
}   

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

void initializeGLFW()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set GLFW error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Set OpenGL version and profile (4.1 core profile)
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL 4.1
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Enable forward compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile
}

void initializeGLEW()
{
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void initializeImGui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void cleanup(GLFWwindow* window)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}