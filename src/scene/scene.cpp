#include "scene.h"

#include <iostream>

#include "../core/utils.h"
#include "../core/model_loader.h"

bool Scene::initialize() 
{
    
    ModelLoader::MeshData meshData;
    if (!ModelLoader::load("../resources/assets/teapot.fbx", meshData))
    {
        std::cerr << "Failed to load OBJ file" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    std::vector<glm::vec3> tangents, bitangents;
    calculateTangents(
        meshData.vertices,
        meshData.uvs,
        meshData.normals,
        meshData.indices,
        tangents,
        bitangents
    );

    if (!m_mesh.initialize(meshData.vertices, meshData.uvs, meshData.normals, meshData.indices, tangents, bitangents))
    {
        std::cerr << "Failed to initialize mesh" << std::endl;
        return false;
    }

    if (!m_diffuseTexture.load("../resources/textures/default.DDS"))
    {
        std::cerr << "Failed to load diffuse texture" << std::endl;
        return false;
    }

    if (!m_normalMap.load("../resources/textures/normal.DDS"))
    {
        std::cerr << "Failed to load normal map" << std::endl;
        return false;
    }  

    m_camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    m_light = Light();
    m_light.setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    m_light.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
    m_light.setPower(20.0f);

    m_transform = Transform();
    m_transform.scale(glm::vec3(0.1f));
    
    return true;
}

void Scene::cleanup()
{
    m_mesh.cleanup();
}

Camera& Scene::getCamera() 
{
    return m_camera;
}

Light& Scene::getLight() 
{
    return m_light;
}

const Mesh& Scene::getMesh() 
{
    return m_mesh;
}

const Transform& Scene::getTransform() 
{
    return m_transform;
}

const Texture& Scene::getDiffuseTexture() 
{
    return m_diffuseTexture;
}

const Texture& Scene::getNormalMap() 
{
    return m_normalMap;
}
