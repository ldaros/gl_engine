#pragma once

#include "scene/scene.h"

namespace Editor {

class EditorUI 
{
public:
    EditorUI();

    void renderPerformanceOverlay();
    void renderEntityInfo(Engine::Scene& scene);
    
private:
    entt::entity m_selectedEntity = entt::null;
};

}
