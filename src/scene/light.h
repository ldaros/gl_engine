#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include <glm/glm.hpp>

enum class LightType
{
    POINT,
    DIRECTIONAL
};

class Light
{
public:
    Light();
	~Light();

    void setPosition(const glm::vec3& position);
    void setDirection(const glm::vec3& direction);
    void setColor(const glm::vec3& color);
	void setPower(float power);
    void setType(LightType type);

    const glm::vec3& getPosition() const { return m_position; }
    const glm::vec3& getDirection() const { return m_direction; }
    const glm::vec3& getColor() const { return m_color; }
	float getPower() const { return m_power; }
    LightType getType() const { return m_type; }
    bool castsShadows() const { return m_type == LightType::DIRECTIONAL; }

private:
    LightType m_type;
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_color;
	float m_power;
};

#endif // LIGHT_H