#include "light.h"

Light::Light()
    : m_type(LightType::POINT)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_direction(0.0f, 0.0f, 0.0f)
    , m_color(1.0f, 1.0f, 1.0f)
    , m_power(1.0f)
{}

Light::~Light() {}

void Light::setPosition(const glm::vec3& position)
{
    m_position = position;
}

void Light::setDirection(const glm::vec3& direction)
{
    m_direction = direction;
}

void Light::setColor(const glm::vec3& color)
{
    m_color = color;
}

void Light::setPower(float power)
{
    m_power = power;
}

void Light::setType(LightType type)
{
    m_type = type;
}
