#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

class ObjLoader 
{
public:
    ObjLoader();
    ~ObjLoader();

    static bool loadOBJ(
        const std::string &path, 
        std::vector<glm::vec3> &outVertices, 
        std::vector<glm::vec2> &outUVs,
        std::vector<glm::vec3> &outNormals,
        std::vector<unsigned int> &outIndices
    );
};

#endif // OBJ_LOADER_H