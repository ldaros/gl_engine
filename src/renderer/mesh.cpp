// mesh.cpp
#include "mesh.h"
#include <GL/glew.h>
#include <iostream>

Mesh::Mesh() { }
Mesh::~Mesh() { }

bool Mesh::initialize(
    const std::vector<glm::vec3> &vertices, 
    const std::vector<glm::vec2> &UVs, 
    const std::vector<glm::vec3> &normals, 
    const std::vector<unsigned int> &indices,
    const std::vector<glm::vec3> &tangents,
    const std::vector<glm::vec3> &bitangents
)
{
    m_indexCount = indices.size();
    m_vertexCount = vertices.size();

    // Generate buffer and array objects
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);  // Vertex positions
    glGenBuffers(1, &tbo);  // Texture coordinates
    glGenBuffers(1, &nbo);  // Normals
    glGenBuffers(1, &tanbo); // Tangents
    glGenBuffers(1, &bitanbo); // Bitangents
    glGenBuffers(1, &ebo);  // Indices

    // Bind the Vertex Array Object first, then bind and set vertex buffers, and then configure vertex attributes.
    glBindVertexArray(vao);

    // Vertex Positions
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    // Vertex Attribute 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); 
    glEnableVertexAttribArray(0);

    // Texture Coordinates
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), UVs.data(), GL_STATIC_DRAW);
    // Vertex Attribute 1
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); 
    glEnableVertexAttribArray(1);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    // Vertex Attribute 2
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); 
    glEnableVertexAttribArray(2);

    // Tangents
    glBindBuffer(GL_ARRAY_BUFFER, tanbo);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);
    // Vertex Attribute 3
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); 
    glEnableVertexAttribArray(3);

    // Bitangents
    glBindBuffer(GL_ARRAY_BUFFER, bitanbo);
    glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), bitangents.data(), GL_STATIC_DRAW);
    // Vertex Attribute 4
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); 
    glEnableVertexAttribArray(4);

    // Element Buffer Object (Indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Unbind the VAO
    glBindVertexArray(0);

    return true;
}

void Mesh::cleanup()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &nbo);
    glDeleteBuffers(1, &tanbo);
    glDeleteBuffers(1, &bitanbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::draw(GLuint shaderProgram, GLuint textureID, GLuint normalMapID) const
{
    // Use the shader program
    glUseProgram(shaderProgram);

    // Bind the diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureDiffuse"), 0);

    // Handle normal map
    bool useNormalMap = normalMapID != 0;
    glUniform1i(glGetUniformLocation(shaderProgram, "useNormalMap"), useNormalMap);

    if (useNormalMap)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMapID);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureNormal"), 1);
    }

    // Draw the mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Cleanup
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Mesh::getIndexCount() const
{
    return m_indexCount;
}

unsigned int Mesh::getVertexCount() const
{
    return m_vertexCount;
}