#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

class ModelLoader 
{
public:
    struct MeshData 
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        std::vector<unsigned int> indices;
    };

    ModelLoader();
    ~ModelLoader();

    static bool load(const std::string &path, MeshData &mesh);
};

#endif // MODEL_LOADER_H