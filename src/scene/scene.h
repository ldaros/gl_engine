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

    void newScene();
    bool loadScene(const std::string& path, ResourceManager& resourceManager);

private:
    entt::registry m_registry;

    TransformComponent deserializeTransform(const json& obj);
    CameraComponent deserializeCamera(const json& obj);
    MeshRendererComponent deserializeMeshRenderer(const json& obj, ResourceManager& resourceManager);
    LightComponent deserializeLight(const json& obj);
};

}
