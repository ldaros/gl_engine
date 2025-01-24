#include "utils.h"

glm::mat4 getModelMatrix(const TransformComponent& transform)
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), transform.position);
    glm::mat4 rotationMatrix = glm::toMat4(transform.rotation);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);
    
    return translationMatrix * rotationMatrix * scaleMatrix;
}

void translate(TransformComponent& transform, const glm::vec3& translation) 
{
    transform.position += translation;
}

void rotate(TransformComponent& transform, const glm::vec3& eulerAngles) 
{
    glm::vec3 radians = glm::radians(eulerAngles);    
    transform.rotation = glm::quat(radians);
}

void scaleUniform(TransformComponent& transform, float uniformScale)
{
    transform.scale = glm::vec3(uniformScale);
}

glm::vec3 forward(const TransformComponent& transform)
{
    return transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 right(const TransformComponent& transform)
{
    return transform.rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 up(const TransformComponent& transform)
{
    return transform.rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

bool calculateTangents(
    const std::vector<glm::vec3> &vertices,
    const std::vector<glm::vec2> &uvs,
    const std::vector<glm::vec3> &normals,
    const std::vector<unsigned int> &indices,
    std::vector<glm::vec3> &out_tangents,
    std::vector<glm::vec3> &out_bitangents
)
{
    // Initialize tangents and bitangents arrays with zeros
    out_tangents.resize(vertices.size(), glm::vec3(0.0f));
    out_bitangents.resize(vertices.size(), glm::vec3(0.0f));

    // Iterate over each triangle
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        // Get the indices of the triangle vertices
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        // Positions
        const glm::vec3 &v0 = vertices[i0];
        const glm::vec3 &v1 = vertices[i1];
        const glm::vec3 &v2 = vertices[i2];

        // UVs
        const glm::vec2 &uv0 = uvs[i0];
        const glm::vec2 &uv1 = uvs[i1];
        const glm::vec2 &uv2 = uvs[i2];

        // Edges of the triangle
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // UV delta
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        // Accumulate the tangents and bitangents for each vertex of the triangle
        out_tangents[i0] += tangent;
        out_tangents[i1] += tangent;
        out_tangents[i2] += tangent;

        out_bitangents[i0] += bitangent;
        out_bitangents[i1] += bitangent;
        out_bitangents[i2] += bitangent;
    }

    // Normalize the tangents and bitangents
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        out_tangents[i] = glm::normalize(out_tangents[i]);
        out_bitangents[i] = glm::normalize(out_bitangents[i]);
    }

    return true;
}
