#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Mesh 
{
    unsigned int indexCount;
public:
    Mesh(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &UVs, std::vector<glm::vec3> &normals);
    ~Mesh();
    void draw(GLuint shaderProgram, GLuint textureID);
    
private:
    unsigned int vao, vbo, tbo, nbo;
    void setupMesh();
};

#endif // MESH_H