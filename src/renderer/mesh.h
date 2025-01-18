// mesh.h
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Mesh 
{
public:
    Mesh();
    ~Mesh();

    bool initialize(
        const std::vector<glm::vec3> &vertices, 
        const std::vector<glm::vec2> &UVs, 
        const std::vector<glm::vec3> &normals, 
        const std::vector<unsigned int> &indices,
        const std::vector<glm::vec3> &tangents,
        const std::vector<glm::vec3> &bitangents
    );
    void cleanup();

    // Render the mesh
    void draw(GLuint shaderProgram, GLuint textureID, GLuint normalMapID = 0) const;

    unsigned int getIndexCount() const;
    unsigned int getVertexCount() const;

private:
    // Mesh Data
    GLuint vao, vbo, tbo, nbo, tanbo, bitanbo, ebo;
    unsigned int m_indexCount;
    unsigned int m_vertexCount;

    // Initializes all the buffer objects/arrays
    void setupMesh();
};

#endif // MESH_H
