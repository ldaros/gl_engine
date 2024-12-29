#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <vector>

class Mesh {
public:
    Mesh(std::vector<float> vertices, std::vector<unsigned int> indices);
    ~Mesh();
    void draw();

private:
    unsigned int vao, vbo, ebo;
    void setupMesh();
};

#endif // MESH_H