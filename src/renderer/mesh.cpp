#include "mesh.h"
#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &UVs, std::vector<glm::vec3> &normals)
    : indexCount(vertices.size())
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &tbo);
    glGenBuffers(1, &nbo);

    glBindVertexArray(vao);

    // Vertex buffer setup
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    // Texture coordinate buffer setup
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

    // Normal buffer setup
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    setupMesh();
}

Mesh::~Mesh() 
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &tbo);
}

void Mesh::setupMesh() 
{
    glBindVertexArray(vao);

    // Vertex buffer setup
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate buffer setup
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // Normal buffer setup
    glBindBuffer(GL_ARRAY_BUFFER, nbo);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::draw(GLuint shaderProgram, GLuint textureID)
{
    // Use the shader program
    glUseProgram(shaderProgram);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    // Bind the VAO before drawing
    glBindVertexArray(vao);
    
    // Draw the mesh (using glDrawArrays since it's a non-indexed mesh)
    glDrawArrays(GL_TRIANGLES, 0, indexCount);

    // Unbind VAO after drawing
    glBindVertexArray(0);

    // Optionally unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}
