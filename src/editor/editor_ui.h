#pragma once

#include "core/sdk.h"

namespace Editor {

class EditorUI 
{
public:
    EditorUI();

    void setupDockingSpace();
    void renderMenuBar(Engine::SDK& sdk);
    void renderSceneView(uintptr_t fb);
    void renderEntityBrowser(Engine::Scene& scene);
    void renderEntityDetails(Engine::Scene& scene);

    std::pair<uint32_t, uint32_t> getFramebufferSize() const { return m_framebufferSize; }
    bool isSceneViewActive() const { return m_isSceneViewActive; }
    
private:
    template <typename ComponentType>
    bool ComponentHeader(entt::registry& registry, entt::entity entity, const char* headerName);

    template <typename ComponentType>
    void AddComponentMenuItem(entt::registry& registry, entt::entity entity, const char* label);

    entt::entity m_selectedEntity = entt::null;
    std::pair<uint32_t, uint32_t> m_framebufferSize { 1920, 1080 };
    bool m_isSceneViewActive = false;
};

}
