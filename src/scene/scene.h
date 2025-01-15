#ifndef SCENE_H
#define SCENE_H

#include "../renderer/mesh.h"
#include "../renderer/light.h"
#include "../renderer/camera.h"
#include "../renderer/texture.h"
#include "transform.h"

class Scene 
{
public:
    bool initialize();
    
    Camera& getCamera();
    Light& getLight();
    const Transform& getTransform();
    const Mesh& getMesh();
    const Texture& getDiffuseTexture();
    const Texture& getNormalMap();

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