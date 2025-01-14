// mesh.cpp
#include "mesh.h"
#include <GL/glew.h>
#include <iostream>

Mesh::Mesh(
    const std::vector<glm::vec3> &vertices, 
    const std::vector<glm::vec2> &UVs, 
    const std::vector<glm::vec3> &normals, 
    const std::vector<unsigned int> &indices
)
    : indexCount(indices.size()), vertexCount(vertices.size())
{
    // Generate buffer and array objects
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);  // Vertex positions
    glGenBuffers(1, &tbo);  // Texture coordinates
    glGenBuffers(1, &nbo);  // Normals
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

    // Element Buffer Object (Indices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Unbind the VAO
    glBindVertexArray(0);
}

Mesh::~Mesh() 
{
    // Delete buffers and arrays
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &tbo);
    glDeleteBuffers(1, &nbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::draw(GLuint shaderProgram, GLuint textureID)
{
    // Use the shader program
    glUseProgram(shaderProgram);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Assume the shader has a sampler2D named "texture1"
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    // Bind the VAO
    glBindVertexArray(vao);

    // Draw the mesh using indices
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);

    // Optionally unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Mesh::getIndexCount() const 
{
    return indexCount;
}

unsigned int Mesh::getVertexCount() const 
{
    return vertexCount;
}