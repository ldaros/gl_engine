#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "uuid.h"

namespace Engine {

struct Image 
{
    std::vector<uint8_t> pixels;
    UUID uuid;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};

struct MeshData
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::vec2> uvs;
    std::vector<uint32_t> indices;
    UUID uuid;
};

struct Material 
{
    std::shared_ptr<Image> albedo;
    std::shared_ptr<Image> normal;
    std::shared_ptr<Image> specular;
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 specularStrength = glm::vec3(0.3f);
    float shininess = 32.0f;
    float opacity = 1.0f;
};

}
