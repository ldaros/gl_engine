// transform.h
#ifndef TRANSFORM_H
#define TRANSFORM_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

class Transform 
{
public:
    Transform();
    
    // Matrix getters
    glm::mat4 getModelMatrix() const;
    glm::mat4 getLocalMatrix() const;
    
    // Position
    void setPosition(const glm::vec3& position);
    void setPosition(float x, float y, float z);
    glm::vec3 getPosition() const;
    void translate(const glm::vec3& translation);
    void translate(float x, float y, float z);
    
    // Rotation
    void setRotation(const glm::vec3& eulerAngles); // In degrees (pitch, yaw, roll)
    void setRotation(float pitch, float yaw, float roll);
    void setRotationQuaternion(const glm::quat& rotation);
    glm::vec3 getRotation() const; // Returns euler angles in degrees
    glm::quat getRotationQuaternion() const;
    void rotate(const glm::vec3& eulerAngles);
    void rotate(float pitch, float yaw, float roll);
    void rotateAround(const glm::vec3& point, const glm::vec3& axis, float angle);
    
    // Scale
    void setScale(const glm::vec3& scale);
    void setScale(float x, float y, float z);
    void setUniformScale(float scale);
    glm::vec3 getScale() const;
    
    // Utility
    void reset();
    glm::vec3 forward() const;
    glm::vec3 right() const;
    glm::vec3 up() const;
    
private:
    void updateMatrices();
    
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_modelMatrix;
    bool m_matrixDirty;
};

#endif // TRANSFORM_H