#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>

class Mesh 
{
    unsigned int indexCount;
public:
    Mesh(std::vector<float>& vertices, std::vector<unsigned int>& indices);
    ~Mesh();
    void draw();

private:
    unsigned int vao, vbo, ebo;
    void setupMesh();
};

#endif // MESH_H