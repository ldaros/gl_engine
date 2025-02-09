#include "scene.h"

#include <fstream>
#include <iostream>
#include "core/assert.h"
#include "core/utils.h"

namespace Engine {
 
using json = nlohmann::json;

void Scene::newScene()
{
    m_registry.clear();
}

bool Scene::loadScene(const std::string& path, ResourceManager& resourceManager)
{
    m_registry.clear();
    resourceManager.cleanup();

    std::ifstream file(path);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open scene file: " << path << std::endl;
        return false;
    }

    try 
    {
        json j;
        file >> j;

        for (auto& e : j["entities"]) 
        {
            auto name = e["name"].get<std::string>();
            auto components = e["components"];

            entt::entity entity = m_registry.create();
            m_registry.emplace<NameComponent>(entity, name);

            for (auto& component : components) 
            {
                auto type = component["type"].get<std::string>();
                auto data = component["data"];

                if (type == "Transform") 
                {
                    m_registry.emplace<TransformComponent>(entity, deserializeTransform(data));
                }
                else if (type == "Camera") 
                {
                    m_registry.emplace<CameraComponent>(entity, deserializeCamera(data));
                }
                else if (type == "Light") 
                {
                    m_registry.emplace<LightComponent>(entity, deserializeLight(data));
                }
                else if (type == "MeshRenderer") 
                {
                    m_registry.emplace<MeshRendererComponent>(entity, deserializeMeshRenderer(data, resourceManager));
                }
                else if (type == "ActiveCamera") 
                {
                    m_registry.emplace<ActiveCamera>(entity);
                }
            }
        }
    }
    catch (json::exception& e) 
    {
        std::cerr << "JSON error in scene file " << path << ":\n"
                  << "  ID: " << e.id << "\n"
                  << "  Message: " << e.what() << std::endl;
        return false;
    }

    return true;
}

TransformComponent Scene::deserializeTransform(const json& obj)
{
    TransformComponent transform;
    transform.position = JsonUtils::parseVec3(obj["position"]);
    transform.rotation = JsonUtils::parseQuat(obj["rotation"]);
    transform.scale = JsonUtils::parseVec3(obj["scale"]);

    return transform;
}

CameraComponent Scene::deserializeCamera(const json& obj)
{
    CameraComponent camera;
    camera.fov = obj["fov"].get<float>();
    camera.nearClip = obj["nearClip"].get<float>();
    camera.farClip = obj["farClip"].get<float>();

    return camera;
}

MeshRendererComponent Scene::deserializeMeshRenderer(const json& obj, ResourceManager& resourceManager)
{
    MeshRendererComponent meshRenderer;

    auto meshDataPath = obj["meshData"].get<std::string>();
    auto materialPath = obj["material"].get<std::string>();

    auto meshData = resourceManager.loadMesh(meshDataPath);
    if (!meshData) 
    {
        return meshRenderer;
    }

    auto material = resourceManager.loadMaterial(materialPath);
    if (!material) 
    {
        return meshRenderer;
    }
    
    meshRenderer.meshData = meshData;
    meshRenderer.material = material;
    meshRenderer.castShadows = obj["castShadows"].get<bool>();

    return meshRenderer;
}

LightComponent Scene::deserializeLight(const json& obj)
{
    LightComponent light;
    light.position = JsonUtils::parseVec3(obj["position"]);
    light.direction = JsonUtils::parseVec3(obj["direction"]);
    light.color = JsonUtils::parseVec3(obj["color"]);
    light.power = obj["power"].get<float>();
    light.type = static_cast<LightType>(obj["type"].get<int>());

    return light;
}

}   