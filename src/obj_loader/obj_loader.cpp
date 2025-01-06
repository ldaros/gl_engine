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

bool ObjLoader::loadOBJ(const std::string &path, std::vector<float> &vertices, std::vector<unsigned int> &indices) 
{
    std::ifstream file(path);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }
    
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempVerticesColors;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempTexCoords;

    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream s(line);
        std::string prefix;
        s >> prefix;

        // Vertex position
        if (prefix == "v") 
        {
            glm::vec3 vertex;
            s >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        }
        // Vertex color
        else if (prefix == "vc") 
        {
            glm::vec3 vertexColor;
            s >> vertexColor.x >> vertexColor.y >> vertexColor.z;
            tempVerticesColors.push_back(vertexColor);
        }
        // Vertex texture coordinate
        else if (prefix == "vt") 
        {
            glm::vec2 texCoord;
            s >> texCoord.x >> texCoord.y;
            tempTexCoords.push_back(texCoord);
        } 
        // Vertex normal
        else if (prefix == "vn") 
        {
            glm::vec3 normal;
            s >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        // Face
        else if (prefix == "f") 
        {
            std::string vertexData;

            // Read vertex indices
            while (s >> vertexData) 
            {
                std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                std::istringstream vertexStream(vertexData);
                unsigned int vIndex, tIndex = 0, nIndex = 0;
                vertexStream >> vIndex;
                if (vertexStream.peek() == ' ') vertexStream >> tIndex;
                if (vertexStream.peek() == ' ') vertexStream >> nIndex;

                // Convert OBJ 1-based index to 0-based index
                indices.push_back(vIndex - 1);
            }
        }
    }

    // Combine vertex attributes (position, normal, texCoord, color)
    for (size_t i = 0; i < tempVertices.size(); ++i) 
    {
        // Vertex positions
        vertices.push_back(tempVertices[i].x);
        vertices.push_back(tempVertices[i].y);
        vertices.push_back(tempVertices[i].z);

        // Vertex colors
        vertices.push_back(tempVerticesColors[i].x);
        vertices.push_back(tempVerticesColors[i].y);
        vertices.push_back(tempVerticesColors[i].z);
    }

    return true;
}
