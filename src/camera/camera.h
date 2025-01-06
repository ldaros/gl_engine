#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera 
{
public:
    Camera(glm::vec3 position, glm::vec3 direction);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float fov, float aspectRatio, float near, float far) const;
    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    glm::vec3 direction;
};

#endif // CAMERA_H
