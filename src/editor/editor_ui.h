#pragma once

#include "scene/scene.h"

namespace Editor {

class EditorUI 
{
public:
    EditorUI();

    void setupDockingSpace();
    void renderSceneView(uintptr_t fb);
    void renderEntityBrowser(Engine::Scene& scene);
    void renderEntityDetails(Engine::Scene& scene);

    std::pair<uint32_t, uint32_t> getFramebufferSize() const { return m_framebufferSize; }
    
private:
    entt::entity m_selectedEntity = entt::null;
    std::pair<uint32_t, uint32_t> m_framebufferSize { 1920, 1080 };
};

}
