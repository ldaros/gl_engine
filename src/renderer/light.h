#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include <glm/glm.hpp>

class Light
{
public:
    Light();
	~Light();

    void setPosition(const glm::vec3& position);
    void setColor(const glm::vec3& color);
	void setPower(float power);

    const glm::vec3& getPosition() const;
    const glm::vec3& getColor() const;
	float getPower() const;

private:
    glm::vec3 m_position;
    glm::vec3 m_color;
	float m_power;
};

#endif // LIGHT_H