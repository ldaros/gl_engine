#include "scene.h"

#include <iostream>

#include "core/utils.h"
#include "core/resource_manager.h"
#include "renderer/material.h"

bool Scene::initialize() 
{
    ResourceManager& resourceManager = ResourceManager::getInstance();

    std::shared_ptr<MeshData> meshData = resourceManager.loadMesh("resources/assets/shadow_test.fbx");
    if (!meshData)
    {
        std::cerr << "Failed to load mesh" << std::endl;
        return false;
    }
    m_mesh.initialize(*meshData);

    std::shared_ptr<ShaderData> shaderData = resourceManager.loadShader(
        "resources/shaders/standard_vs.glsl",
        "resources/shaders/standard_fs.glsl"
    );
    if (!shaderData)
    {
        std::cerr << "Failed to load shader" << std::endl;
        return false;
    }
    m_shader = std::make_shared<Shader>();
    if(!m_shader->init(shaderData->vertexCode, shaderData->fragmentCode))
    {
        std::cerr << "Failed to compile shader" << std::endl;
        return false;
    }

    m_diffuseTexture = resourceManager.loadTexture("resources/textures/default.png");
    if (!m_diffuseTexture)
    {
        std::cerr << "Failed to load diffuse texture" << std::endl;
        return false;
    }

    m_normalMap = resourceManager.loadTexture("resources/textures/normal.png");
    if (!m_normalMap)
    {
        std::cerr << "Failed to load normal map texture" << std::endl;
    }

    m_material.shader = m_shader;
    m_material.diffuseTexture = m_diffuseTexture;
    // m_material.normalMap = m_normalMap;

    m_camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f), -90.0f, 0.0f);
    m_camera.setRotation(0.0f, 0.0f);

    m_light = Light();
    m_light.setType(LightType::DIRECTIONAL);
    m_light.setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    m_light.setDirection(glm::vec3(0.5f, -0.5f, -1.0f));
    m_light.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    m_light.setPower(1.0f);

    m_transform.scale = glm::vec3(0.1f);
    
    return true;
}

void Scene::cleanup()
{
    m_mesh.cleanup();
}

Transform& Scene::getTransform()
{
    return m_transform;
}

Camera& Scene::getCamera()
{
    return m_camera;
}

Light& Scene::getLight()
{
    return m_light;
}
