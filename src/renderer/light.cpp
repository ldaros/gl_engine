#include "light.h"

Light::Light()
    : position(0.0f, 0.0f, 0.0f),
      color(1.0f, 1.0f, 1.0f),
      power(1.0f)
{}

Light::~Light() {}

void Light::setPosition(const glm::vec3& position)
{
    this->position = position;
}

void Light::setColor(const glm::vec3& color)
{
    this->color = color;
}

void Light::setPower(float power)
{
    this->power = power;
}

const glm::vec3& Light::getPosition() const
{
    return position;
}

const glm::vec3& Light::getColor() const
{
    return color;
}

float Light::getPower() const
{
    return power;
}