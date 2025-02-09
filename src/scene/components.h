#pragma once

#include <memory>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include "core/resources.h"

namespace Engine {

struct Renderable {};
struct LightSource {};
struct PerspectiveCamera {};
struct ActiveCamera {};

struct NameComponent
{
    std::string name;
};

struct TransformComponent
{
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
};

struct MeshRendererComponent 
{
    std::shared_ptr<MeshData> meshData;
    std::shared_ptr<Material> material;
    bool castShadows = true;
};

enum class LightType : uint8_t
{
    POINT,
    DIRECTIONAL
};

struct LightComponent
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float power = 1.0f;
    LightType type = LightType::POINT;
};

struct CameraComponent 
{
    float fov = 45.0f;
    float nearClip = 0.1f;
    float farClip = 1000.0f;
};

}  
