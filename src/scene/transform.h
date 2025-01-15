#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform 
{
public:
    Transform();
    glm::mat4 getModelMatrix() const;
    void rotate(float angle, glm::vec3 axis);
    void translate(glm::vec3 translation);
    void scale(glm::vec3 scale);
    void reset();
    
private:
    glm::mat4 modelMatrix;
};

#endif // TRANSFORM_H
