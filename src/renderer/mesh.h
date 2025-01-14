// mesh.h
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Mesh {
public:
    // Constructor & Destructor
    Mesh(
        const std::vector<glm::vec3> &vertices, 
        const std::vector<glm::vec2> &UVs, 
        const std::vector<glm::vec3> &normals, 
        const std::vector<unsigned int> &indices
    );
    ~Mesh();

    // Render the mesh
    void draw(GLuint shaderProgram, GLuint textureID);

    unsigned int getIndexCount() const;
    unsigned int getVertexCount() const;

private:
    // Mesh Data
    GLuint vao, vbo, tbo, nbo, ebo;
    unsigned int indexCount;
    unsigned int vertexCount;

    // Initializes all the buffer objects/arrays
    void setupMesh();
};

#endif // MESH_H
