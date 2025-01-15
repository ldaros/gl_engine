#include "light.h"

Light::Light()
    : m_position(0.0f, 0.0f, 0.0f),
      m_color(1.0f, 1.0f, 1.0f),
      m_power(1.0f)
{}

Light::~Light() {}

void Light::setPosition(const glm::vec3& position)
{
    this->m_position = position;
}

void Light::setColor(const glm::vec3& color)
{
    this->m_color = color;
}

void Light::setPower(float power)
{
    this->m_power = power;
}

const glm::vec3& Light::getPosition() const
{
    return m_position;
}

const glm::vec3& Light::getColor() const
{
    return m_color;
}

float Light::getPower() const
{
    return m_power;
}