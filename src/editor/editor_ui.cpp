#include "editor_ui.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include "tinyfiledialogs.h"
#include "scene/components.h"

namespace Editor {

using namespace Engine;

EditorUI::EditorUI() {}

void EditorUI::setupDockingSpace()
{
    // Create main docking space
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags dockspaceFlags = 
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("MainDockspace", nullptr, dockspaceFlags);
    ImGui::PopStyleVar();

    ImGuiID dockspaceID = ImGui::GetID("MainDockspace");
    if (ImGui::DockBuilderGetNode(dockspaceID) == nullptr)
    {
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        // Split main dockspace into left (25%) and right (75%)
        ImGuiID left, right;
        ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.25f, &left, &right);

        // Split left side into top (Entity Browser) and bottom (Entity Details)
        ImGuiID top, bottom;
        ImGui::DockBuilderSplitNode(left, ImGuiDir_Up, 0.5f, &top, &bottom);

        // Assign windows to docks
        ImGui::DockBuilderDockWindow("Entity Browser", top);
        ImGui::DockBuilderDockWindow("Entity Details", bottom);
        ImGui::DockBuilderDockWindow("Scene View", right);
        
        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void EditorUI::renderMenuBar(SDK& sdk)
{
    ImGui::BeginMainMenuBar();
    {
        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("New Scene")) 
            {
                sdk.scene->newScene();
            }
            
            if (ImGui::MenuItem("Open Scene"))
            {
                const char* filters[] = { "*.json", "*.scene" };
                const char* filename = tinyfd_openFileDialog(
                    "Open Scene",  // Title
                    "",            // Default path
                    2,             // Number of filters
                    filters,       // Filter array
                    "Scene Files", // Filter description
                    0              // Allow multiple selections (0 = no)
                );
                
                if (filename != nullptr)
                {
                    sdk.scene->loadScene(filename, *sdk.resourceManager);
                }
            }            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Editor"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4")) 
            {
                sdk.window->close();
            }

            ImGui::EndMenu();
        }
    }    
    ImGui::EndMainMenuBar();
}

void EditorUI::renderSceneView(uintptr_t fb)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
    {   
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        m_framebufferSize = {
            static_cast<uint32_t>(contentSize.x),
            static_cast<uint32_t>(contentSize.y)
        };

        ImGui::Image(
            static_cast<ImTextureID>(fb), 
            contentSize, 
            ImVec2(0, 1), 
            ImVec2(1, 0)
        );
        
        // Performance overlay
        ImGui::SetCursorPos(ImVec2(10, 30));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.35f));
        if (ImGui::BeginChild("##PerfOverlay", ImVec2(0, 0), 
            ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
        )) {
            float fps = ImGui::GetIO().Framerate;
            float msPerFrame = 1000.0f / fps;
            ImGui::Text("%.1f FPS (%.3f ms/frame)", fps, msPerFrame);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        
        m_isSceneViewActive = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void EditorUI::renderEntityBrowser(Engine::Scene& scene)
{
    auto& registry = scene.getRegistry();

    ImGui::Begin("Entity Browser");
    {
        for(auto entity: registry.view<entt::entity>()) 
        {
            // Get name or generate ID string
            std::string displayName;
            if (auto name = registry.try_get<NameComponent>(entity)) 
            {
                displayName = name->name;
            } 
            else 
            {
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
    ImGui::End();    
}

void EditorUI::renderEntityDetails(Engine::Scene& scene)
{
    auto& registry = scene.getRegistry();

    ImGui::Begin("Entity Details");
    {
        if (registry.valid(m_selectedEntity))
        {   
            const auto entityId = static_cast<uint32_t>(m_selectedEntity);
            std::string headerText = "Entity";

            if (auto name = registry.try_get<NameComponent>(m_selectedEntity)) 
            {
                headerText = name->name;
            } 
            else {
                headerText += " #" + std::to_string(entityId);
            }

            ImGui::Text("%s", headerText.c_str());
            ImGui::Separator();

            // Transform Component
            if (auto* transform = registry.try_get<TransformComponent>(m_selectedEntity)) 
            {
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) 
                {
                    ImGui::DragFloat3("Position", &transform->position[0], 0.1f);
                    glm::vec3 degrees = glm::degrees(glm::eulerAngles(transform->rotation));
                    ImGui::DragFloat3("Rotation", &degrees[0], 0.1f);
                    transform->rotation = glm::quat(glm::radians(degrees));
                    ImGui::DragFloat3("Scale", &transform->scale[0], 0.1f);
                }
            }

            // Camera Component
            if (auto* camera = registry.try_get<CameraComponent>(m_selectedEntity)) 
            {
                if (ImGui::CollapsingHeader("Camera")) 
                {
                    ImGui::DragFloat("FOV", &camera->fov, 0.1f, 1.0f, 180.0f);
                    ImGui::DragFloat("Near Plane", &camera->nearClip, 0.01f, 0.01f, 10.0f);
                    ImGui::DragFloat("Far Plane", &camera->farClip, 1.0f, 10.0f, 10000.0f);
                }
            }

            // Mesh Renderer Component
            if (auto* meshRenderer = registry.try_get<MeshRendererComponent>(m_selectedEntity)) 
            {
                if (ImGui::CollapsingHeader("Mesh Renderer")) 
                {
                    ImGui::Checkbox("Cast Shadows", &meshRenderer->castShadows);
                }
            }

            // Light Component
            if (auto* light = registry.try_get<LightComponent>(m_selectedEntity)) 
            {
                if (ImGui::CollapsingHeader("Light")) 
                {
                    ImGui::DragFloat3("LightPosition", &light->position[0], 0.1f);
                    ImGui::DragFloat3("LightDirection", &light->direction[0], 0.1f);
                    ImGui::ColorEdit3("Color", &light->color[0]);
                    ImGui::DragFloat("Intensity", &light->power, 0.1f, 0.0f, 100.0f);
                    
                    const char* lightTypes[] = { "Point Light", "Directional Light" };
                    static int selectedType = static_cast<int>(light->type);
                    ImGui::Combo("Light Type", &selectedType, lightTypes, IM_ARRAYSIZE(lightTypes));

                    // Update light type based on selection
                    if (selectedType == 0) 
                    {
                        light->type = LightType::POINT;
                    } 
                    else if (selectedType == 1) 
                    {
                        light->type = LightType::DIRECTIONAL;
                    }
                }
            }
        }
        else 
        {
            ImGui::Text("No entity selected");
        }
    }
    ImGui::End();
}

}