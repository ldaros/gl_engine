#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <vector>

bool calculateTangents(
    const std::vector<glm::vec3> &vertices,
    const std::vector<glm::vec2> &uvs,
    const std::vector<glm::vec3> &normals,
    const std::vector<unsigned int> &indices,
    std::vector<glm::vec3> &out_tangents,
    std::vector<glm::vec3> &out_bitangents
);

#endif // UTILS_H