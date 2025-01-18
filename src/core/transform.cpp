// transform.cpp
#include "transform.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

Transform::Transform()
    : m_position(0.0f)
    , m_rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , m_scale(1.0f)
    , m_modelMatrix(1.0f)
    , m_matrixDirty(false)
{}

void Transform::updateMatrices()
{
    if (m_matrixDirty) 
    {
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position);
        glm::mat4 rotationMatrix = glm::toMat4(m_rotation);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);
        
        m_modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        m_matrixDirty = false;
    }
}

glm::mat4 Transform::getModelMatrix() const 
{
    const_cast<Transform*>(this)->updateMatrices();
    return m_modelMatrix;
}

void Transform::setPosition(const glm::vec3& newPosition) 
{
    m_position = newPosition;
    m_matrixDirty = true;
}

void Transform::setPosition(float x, float y, float z) 
{
    setPosition(glm::vec3(x, y, z));
}

glm::vec3 Transform::getPosition() const 
{
    return m_position;
}

void Transform::translate(const glm::vec3& translation) 
{
    m_position += translation;
    m_matrixDirty = true;
}

void Transform::translate(float x, float y, float z) 
{
    translate(glm::vec3(x, y, z));
}

void Transform::setRotation(const glm::vec3& eulerAngles) 
{
    // Convert degrees to radians
    glm::vec3 radians = glm::radians(eulerAngles);
    
    // Create quaternion from euler angles (YXZ order)
    m_rotation = glm::quat(radians);
    m_matrixDirty = true;
}

void Transform::setRotation(float pitch, float yaw, float roll) 
{
    setRotation(glm::vec3(pitch, yaw, roll));
}

void Transform::setRotationQuaternion(const glm::quat& newRotation) 
{
    m_rotation = newRotation;
    m_matrixDirty = true;
}

glm::vec3 Transform::getRotation() const 
{
    // Convert quaternion to euler angles in degrees
    return glm::degrees(glm::eulerAngles(m_rotation));
}

glm::quat Transform::getRotationQuaternion() const 
{
    return m_rotation;
}

void Transform::rotate(const glm::vec3& eulerAngles) 
{
    glm::quat deltaRotation = glm::quat(glm::radians(eulerAngles));
    m_rotation = deltaRotation * m_rotation;
    m_matrixDirty = true;
}

void Transform::rotate(float pitch, float yaw, float roll)
{
    rotate(glm::vec3(pitch, yaw, roll));
}

void Transform::rotateAround(const glm::vec3& point, const glm::vec3& axis, float angle)
{
    // Convert angle to radians
    float radians = glm::radians(angle);
    
    // Create rotation quaternion
    glm::quat deltaRotation = glm::angleAxis(radians, glm::normalize(axis));
    
    // Transform position relative to point
    glm::vec3 relativePos = m_position - point;
    relativePos = deltaRotation * relativePos;
    m_position = point + relativePos;
    
    // Apply rotation
    m_rotation = deltaRotation * m_rotation;
    m_matrixDirty = true;
}

void Transform::setScale(const glm::vec3& newScale)
{
    m_scale = newScale;
    m_matrixDirty = true;
}

void Transform::setScale(float x, float y, float z)
{
    setScale(glm::vec3(x, y, z));
}

void Transform::setUniformScale(float uniformScale)
{
    setScale(glm::vec3(uniformScale));
}

glm::vec3 Transform::getScale() const
{
    return m_scale;
}

void Transform::reset()
{
    m_position = glm::vec3(0.0f);
    m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    m_scale = glm::vec3(1.0f);
    m_modelMatrix = glm::mat4(1.0f);
    m_matrixDirty = false;
}

glm::vec3 Transform::forward() const
{
    return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Transform::right() const
{
    return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform::up() const
{
    return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}
