#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "file_manager/file_manager.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "transform/transform.h"
#include "mesh/mesh.h"
#include "obj_loader/obj_loader.h"

void glfwErrorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
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
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    bool showGui = true;

    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    glEnable(GL_CULL_FACE); // Enable culling
    glCullFace(GL_BACK);    // Cull back faces
    glFrontFace(GL_CCW);    // Counterclockwise faces are treated as front

    // Initialize shader
    Shader shader;
    shader.init(
        FileManager::read("../src/shaders/vertex_shader.glsl"),  // Vertex shader source
        FileManager::read("../src/shaders/fragment_shader.glsl") // Fragment shader source
    );

    // Load OBJ file
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    if (!ObjLoader::loadOBJ("../assets/pyramid.obj", vertices, indices))
    {
        std::cerr << "Failed to load OBJ file" << std::endl;
        return -1;
    }
    Mesh mesh(vertices, indices);

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
        shader.use();
        shader.setMat4("u_model", transform.getModelMatrix());
        shader.setMat4("u_view", view);
        shader.setMat4("u_projection", projection);

        // Draw mesh
        mesh.draw();
        
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