#include "scene.h"

#include <iostream>

#include "core/utils.h"
#include "core/model_loader.h"
#include "core/image_loader.h"

bool Scene::initialize() 
{
    ModelLoader::MeshData meshData;
    if (!ModelLoader::load("resources/assets/shadow_test.fbx", meshData))
    {
        std::cerr << "Failed to load OBJ file" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    if (!m_mesh.initialize(
        meshData.vertices,
        meshData.uvs,
        meshData.normals,
        meshData.indices,
        meshData.tangents,
        meshData.bitangents))
    {
        std::cerr << "Failed to initialize mesh" << std::endl;
        return false;
    }

    ImageLoader::ImageData diffuseTextureData;
    if (!ImageLoader::load("resources/textures/default.png", diffuseTextureData))
    {
        std::cerr << "Failed to load diffuse texture" << std::endl;
        return false;
    }
    m_diffuseTexture.initialize(diffuseTextureData);

    ImageLoader::ImageData normalMapData;
    if (!ImageLoader::load("resources/textures/normal.png", normalMapData))
    {
        std::cerr << "Failed to load normal map" << std::endl;
        return false;
    }
    // m_normalMap.initialize(normalMapData);

    m_camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    m_light = Light();
    m_light.setType(LightType::DIRECTIONAL);
    m_light.setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    m_light.setDirection(glm::vec3(0.5f, -0.5f, -1.0f));
    m_light.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    m_light.setPower(1.0f);

    m_transform.setScale(glm::vec3(0.1f));
    
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
