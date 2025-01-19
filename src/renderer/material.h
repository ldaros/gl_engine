#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#include "shader.h"
#include "texture.h"

struct Material 
{
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> diffuseTexture;
    std::shared_ptr<Texture> normalMap;
};

#endif // MATERIAL_H