// mesh.h
#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "core/resource_manager.h"

class Mesh 
{
public:
    Mesh();
    ~Mesh();

    bool initialize(const MeshData& meshData);
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
