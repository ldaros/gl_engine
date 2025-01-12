#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>

class Mesh 
{
    unsigned int indexCount;
public:
    Mesh(std::vector<float>& vertices, std::vector<float>& texCoords);
    ~Mesh();
    void draw(GLuint shaderProgram, GLuint textureID);
    
private:
    unsigned int vao, vbo, tbo;
    void setupMesh();
};

#endif // MESH_H