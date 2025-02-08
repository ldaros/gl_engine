#pragma once

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "scene/components.h"
#include "nlohmann/json.hpp"

namespace Engine {

class MathUtils
{
public:
    static glm::mat4 calculateModelMatrix(const TransformComponent& transform);

    static void translate(TransformComponent& transform, const glm::vec3& translation);
    static void rotate(TransformComponent& transform, const glm::vec3& eulerAngles);
    static void scaleUniform(TransformComponent& transform, float uniformScale);
    static void resetTransform(TransformComponent& transform);

    static glm::vec3 forward(const TransformComponent& transform);
    static glm::vec3 right(const TransformComponent& transform);
    static glm::vec3 up(const TransformComponent& transform);

    static int calculateNumberOfMipmaps(int width, int height);
};


class JsonUtils
{
public:
    static glm::vec3 parseVec3(const nlohmann::json& obj);
};

}
