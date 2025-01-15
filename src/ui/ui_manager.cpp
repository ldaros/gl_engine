#include "ui_manager.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>

void UIManager::initialize(GLFWwindow* window) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // Configure ImGui style
    ImGui::StyleColorsDark();
    
    // Initialize ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    
    // Configure ImGui IO
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard controls
}

void UIManager::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::render(Scene& scene) {
    renderMainOverlay(scene);

    if (m_showLightingWindow) renderLightingWindow(scene);
    if (m_showPerformanceWindow) renderPerformanceWindow();

    UIManager::renderFrame();
}

void UIManager::renderMainOverlay(Scene& scene) {
    // Configure window flags
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoMove;
    
    // Position window in top-left corner with some padding
    const float PADDING = 10.0f;
    ImGui::SetNextWindowPos(ImVec2(PADDING, PADDING), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);
    
    if (ImGui::Begin("Main Overlay", nullptr, window_flags)) {
        // FPS counter
        ImGui::Text("%.1f FPS (%.3f ms/frame)", 
            ImGui::GetIO().Framerate,
            1000.0f / ImGui::GetIO().Framerate);
        
        ImGui::Separator();
        
        // Basic scene info
        const Mesh& mesh = scene.getMesh();
        ImGui::Text("Vertices: %d", mesh.getVertexCount());
        ImGui::Text("Triangles: %d", mesh.getIndexCount() / 3);
    }
    ImGui::End();
}

void UIManager::renderLightingWindow(Scene& scene) {
    if (ImGui::Begin("Lighting", &m_showLightingWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
        Light& light = scene.getLight();
        LightType type = light.getType();

        if (ImGui::RadioButton("Point", type == LightType::POINT)) {
            light.setType(LightType::POINT);
        }

        if (ImGui::RadioButton("Directional", type == LightType::DIRECTIONAL)) {
            light.setType(LightType::DIRECTIONAL);
        }

        glm::vec3 position = light.getPosition();
        if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
            light.setPosition(position);
        }

        glm::vec3 direction = light.getDirection();
        if (ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.1f)) {
            light.setDirection(direction);
        }

        glm::vec3 color = light.getColor();
        if (ImGui::ColorEdit3("Color", glm::value_ptr(color))) {
            light.setColor(color);
        }
        
        float power = light.getPower();
        if (ImGui::SliderFloat("Power", &power, 0.0f, 100.0f)) {
            light.setPower(power);
        }
    }
    ImGui::End();
}

void UIManager::renderPerformanceWindow() {
    if (ImGui::Begin("Performance", &m_showPerformanceWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
        const float HISTORY_SIZE = 90;
        static float fpsHistory[90] = {};
        static int offset = 0;
        
        // Update FPS history
        fpsHistory[offset] = ImGui::GetIO().Framerate;
        offset = (offset + 1) % IM_ARRAYSIZE(fpsHistory);
        
        // Calculate stats
        float average = 0.0f;
        float min = FLT_MAX;
        float max = 0.0f;
        for (int n = 0; n < IM_ARRAYSIZE(fpsHistory); n++) {
            if (fpsHistory[n] > 0.0f) {
                average += fpsHistory[n];
                min = std::min(min, fpsHistory[n]);
                max = std::max(max, fpsHistory[n]);
            }
        }
        average /= HISTORY_SIZE;
        
        // Display stats
        ImGui::Text("Average: %.1f FPS", average);
        ImGui::Text("Min: %.1f FPS", min);
        ImGui::Text("Max: %.1f FPS", max);
        
        // Plot FPS graph
        ImGui::PlotLines("FPS", fpsHistory, IM_ARRAYSIZE(fpsHistory), offset, 
                        nullptr, 0.0f, max * 1.2f, ImVec2(0, 80.0f));
    }
    ImGui::End();
}


void UIManager::renderFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::toggleLightingWindow() {
    m_showLightingWindow = !m_showLightingWindow;
}

void UIManager::togglePerformanceWindow() {
    m_showPerformanceWindow = !m_showPerformanceWindow;
}