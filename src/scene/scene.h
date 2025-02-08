#pragma once

#include <string>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include "core/resource_manager.h"
#include "components.h"

namespace Engine {

using json = nlohmann::json;

class Scene 
{
public:
    Scene() = default;
    
    entt::registry& getRegistry()
    {
        return m_registry;
    }

    entt::entity findEntityByName(const std::string& name)
    {
        auto view = m_registry.view<NameComponent>();
        for(auto entity : view) 
        {
            if(view.get<NameComponent>(entity).name == name) 
            {
                return entity;
            }
        }
        return entt::null;
    }

    entt::entity getActiveCamera() const 
    {
        auto view = m_registry.view<CameraComponent>();
        for(auto entity : view) 
        {
            if(view.get<CameraComponent>(entity).active) 
            {
                return entity;
            }
        }
        return entt::null;
    }

    bool loadScene(const std::string& path, ResourceManager& resourceManager);

private:
    entt::registry m_registry;

    TransformComponent deserializeTransform(const json& obj);
    CameraComponent deserializeCamera(const json& obj);
    MeshRendererComponent deserializeMeshRenderer(const json& obj, ResourceManager& resourceManager);
    LightComponent deserializeLight(const json& obj);
};

}
