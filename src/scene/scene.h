#ifndef SCENE_H
#define SCENE_H

#include "renderer/mesh.h"
#include "renderer/texture.h"
#include "renderer/material.h"
#include "renderer/shader.h"
#include "core/transform.h"
#include "camera.h"
#include "light.h"

class Scene 
{
public:
    bool initialize();
    
    Camera& getCamera();
    Light& getLight();
    Transform& getTransform();
    const Mesh& getMesh() const { return m_mesh; }
    const Material& getMaterial() const { return m_material; }

    void cleanup();

private:
    Camera m_camera;
    Light m_light;
    Mesh m_mesh;
    Transform m_transform;
    Material m_material;
    
    std::shared_ptr<Texture> m_diffuseTexture;
    std::shared_ptr<Texture> m_normalMap;
    std::shared_ptr<Shader> m_shader;
};

#endif // SCENE_H