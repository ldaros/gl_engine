#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {
public:
    Transform();
    glm::mat4 getModelMatrix() const;
    void rotate(float angle, glm::vec3 axis);

private:
    glm::mat4 modelMatrix;
};

#endif // TRANSFORM_H
