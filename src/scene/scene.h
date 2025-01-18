#ifndef SCENE_H
#define SCENE_H

#include "../renderer/mesh.h"
#include "../renderer/light.h"
#include "../renderer/camera.h"
#include "../renderer/texture.h"
#include "../core/transform.h"

class Scene 
{
public:
    bool initialize();
    
    Camera& getCamera();
    Light& getLight();
    Transform& getTransform();
    const Mesh& getMesh() const { return m_mesh; }
    const Texture& getDiffuseTexture() const { return m_diffuseTexture; }
    const Texture& getNormalMap() const { return m_normalMap; }

    void cleanup();

private:
    Camera m_camera;
    Light m_light;
    Mesh m_mesh;
    Transform m_transform;
    Texture m_diffuseTexture;
    Texture m_normalMap;
};

#endif // SCENE_H