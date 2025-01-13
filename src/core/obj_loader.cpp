#include "obj_loader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>

ObjLoader::ObjLoader() {}

ObjLoader::~ObjLoader() {}

bool ObjLoader::loadOBJ(
    const std::string &path, 
    std::vector<glm::vec3> &outVertices,
    std::vector<glm::vec2> &outUVs,
    std::vector<glm::vec3> &outNormals
)
{
    std::cout << "Loading OBJ file: " << path << "...\n";

    // Temporary storage for parsing
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    // Open the file
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open the file. Please check the path: " << path << "\n";
        return false;
    }

    // Parse the file
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string prefix;
        lineStream >> prefix;

        if (prefix == "v") {
            // Vertex position
            glm::vec3 vertex;
            lineStream >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        } else if (prefix == "vt") {
            // Texture coordinate
            glm::vec2 uv;
            lineStream >> uv.x >> uv.y;
            uv.y = -uv.y; // Invert V coordinate for DDS textures
            tempUvs.push_back(uv);
        } else if (prefix == "vn") {
            // Normal vector
            glm::vec3 normal;
            lineStream >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        } else if (prefix == "f") {
            // Face
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            char slash; // To handle the '/' in the face definitions
            for (int i = 0; i < 3; ++i) {
                lineStream >> vertexIndex[i] >> slash >> uvIndex[i] >> slash >> normalIndex[i];
            }
            vertexIndices.insert(vertexIndices.end(), { vertexIndex[0], vertexIndex[1], vertexIndex[2] });
            uvIndices.insert(uvIndices.end(), { uvIndex[0], uvIndex[1], uvIndex[2] });
            normalIndices.insert(normalIndices.end(), { normalIndex[0], normalIndex[1], normalIndex[2] });
        } else {
            // Skip other lines (comments, empty lines, etc.)
            continue;
        }
    }

    file.close();
    
    // Populate output vectors
    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        outVertices.push_back(tempVertices[vertexIndices[i] - 1]);
        outUVs.push_back(tempUvs[uvIndices[i] - 1]);
        outNormals.push_back(tempNormals[normalIndices[i] - 1]);
    }

    std::cout << "Successfully loaded OBJ file.\n";
    return true;
}
