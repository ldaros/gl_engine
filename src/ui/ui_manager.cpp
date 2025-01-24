#include "ui_manager.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>

void UIManager::initialize(GLFWwindow* window) 
{
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

void UIManager::newFrame() 
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::render(Scene& scene) 
{
    UIManager::renderPerformanceOverlay();
    UIManager::renderEntityInfo(scene);
    UIManager::renderFrame();
}

void UIManager::renderPerformanceOverlay() 
{
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
    
    if (ImGui::Begin("Performance Overlay", nullptr, window_flags)) 
    {
        // Basic FPS display
        ImGui::Text("%.1f FPS (%.3f ms/frame)", 
            ImGui::GetIO().Framerate,
            1000.0f / ImGui::GetIO().Framerate);

        // FPS history and statistics
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
        for (int n = 0; n < IM_ARRAYSIZE(fpsHistory); n++) 
        {
            if (fpsHistory[n] > 0.0f) 
            {
                average += fpsHistory[n];
                min = std::min(min, fpsHistory[n]);
                max = std::max(max, fpsHistory[n]);
            }
        }
        average /= HISTORY_SIZE;
        
        // Display stats
        ImGui::Separator();
        ImGui::Text("Average: %.1f FPS", average);
        ImGui::Text("Min: %.1f FPS", min);
        ImGui::Text("Max: %.1f FPS", max);
        
        // Plot FPS graph
        ImGui::PlotLines("FPS History", fpsHistory, IM_ARRAYSIZE(fpsHistory), offset, 
                        nullptr, 0.0f, max * 1.2f, ImVec2(0, 80.0f));
    }
    ImGui::End();
}

void UIManager::renderEntityInfo(Scene& scene)
{
    if (ImGui::Begin("Entity Browser", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        auto& registry = scene.getRegistry();
        
        // Entity list
        ImGui::BeginChild("EntityList", ImVec2(300, 300), true);
        {
            for(auto entity: registry.view<entt::entity>()) {
                // Get name or generate ID string
                std::string displayName;
                if (auto name = registry.try_get<NameComponent>(entity)) {
                    displayName = name->name;
                } else {
                    displayName = std::to_string(entt::to_integral(entity));
                }

                ImGui::PushID(static_cast<int>(entt::to_integral(entity)));
                
                // Selectable entity entry
                if (ImGui::Selectable(displayName.c_str(), m_selectedEntity == entity)) {
                    m_selectedEntity = entity;
                }
                
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        // Component details for selected entity
        // ImGui::SameLine();
        ImGui::BeginChild("ComponentDetails", ImVec2(300, 300), true);
        {
            if (registry.valid(m_selectedEntity)) {   
                const auto entityId = static_cast<uint32_t>(m_selectedEntity);
                std::string headerText = "Entity";

                if (auto name = registry.try_get<NameComponent>(m_selectedEntity)) {
                    headerText = name->name;
                } else {
                    headerText += " #" + std::to_string(entityId);
                }

                ImGui::Text("%s", headerText.c_str());
                ImGui::Separator();

                // Transform Component
                if (auto* transform = registry.try_get<TransformComponent>(m_selectedEntity)) {
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::DragFloat3("Position", &transform->position[0], 0.1f);

                        glm::vec3 degrees = glm::degrees(glm::eulerAngles(transform->rotation));
                        ImGui::DragFloat3("Rotation", &degrees[0], 0.1f);
                        transform->rotation = glm::quat(glm::radians(degrees));

                        ImGui::DragFloat3("Scale", &transform->scale[0], 0.1f);
                    }
                }

                // Camera Component
                if (auto* camera = registry.try_get<CameraComponent>(m_selectedEntity)) {
                    if (ImGui::CollapsingHeader("Camera")) {
                        ImGui::DragFloat("FOV", &camera->fov, 0.1f, 1.0f, 180.0f);
                        ImGui::DragFloat("Near Plane", &camera->nearClip, 0.01f, 0.01f, 10.0f);
                        ImGui::DragFloat("Far Plane", &camera->farClip, 1.0f, 10.0f, 10000.0f);
                        ImGui::Checkbox("Active", &camera->active);
                    }
                }

                // Mesh Renderer Component
                if (auto* meshRenderer = registry.try_get<MeshRendererComponent>(m_selectedEntity)) {
                    if (ImGui::CollapsingHeader("Mesh Renderer")) {
                        ImGui::Checkbox("Cast Shadows", &meshRenderer->castShadows);
                
                        if (
                            meshRenderer->material->diffuseTexture && 
                            meshRenderer->material->diffuseTexture->glTextureId != 0
                        ) {
                            ImGui::Text("Diffuse");
                            ImGui::Image(meshRenderer->material->diffuseTexture->glTextureId, ImVec2(50, 50));  
                        }
                        
                        if (
                            meshRenderer->material->normalMap && 
                            meshRenderer->material->normalMap->glTextureId != 0
                        ) {
                            ImGui::Text("Normal Map");
                            ImGui::Image(meshRenderer->material->normalMap->glTextureId, ImVec2(50, 50));  
                        }
                    }
                }

                // Light Component
                if (auto* light = registry.try_get<LightComponent>(m_selectedEntity)) {
                    if (ImGui::CollapsingHeader("Light")) {
                        ImGui::DragFloat3("Position", &light->position[0], 0.1f);
                        ImGui::DragFloat3("Direction", &light->direction[0], 0.1f);
                        ImGui::ColorEdit3("Color", &light->color[0]);
                        ImGui::DragFloat("Intensity", &light->power, 0.1f, 0.0f, 100.0f);
                        
                        const char* lightTypes[] = { "Point Light", "Directional Light" };
                        static int selectedType = static_cast<int>(light->type);
                        ImGui::Combo("Light Type", &selectedType, lightTypes, IM_ARRAYSIZE(lightTypes));

                        // Update light type based on selection
                        if (selectedType == 0) {
                            light->type = LightType::POINT;
                        } else if (selectedType == 1) {
                            light->type = LightType::DIRECTIONAL;
                        }
                    }
                }
            }
            else {
                ImGui::Text("No entity selected");
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}


void UIManager::renderFrame() 
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::cleanup() 
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
