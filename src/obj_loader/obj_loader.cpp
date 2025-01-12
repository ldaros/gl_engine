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

bool ObjLoader::loadOBJ(const std::string &path, std::vector<float> &vertices, std::vector<float> &texCoords)
{
    std::ifstream file(path);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }
    
    std::vector<glm::vec3> tempVertices;
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
        // Vertex texture coordinate
        else if (prefix == "vt") 
        {
            glm::vec2 texCoord;
            s >> texCoord.x >> texCoord.y;
            texCoord.y = 1.0f - texCoord.y; // Flip V-coordinate
            tempTexCoords.push_back(texCoord);
        } 
        // Face
        else if (prefix == "f") 
        {
            std::string vertexData;

            // Read vertex data for each face element
            while (s >> vertexData) 
            {
                // Replace '/' with spaces so we can extract indices
                std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                std::istringstream vertexStream(vertexData);

                unsigned int vIndex, tIndex = 0;

                vertexStream >> vIndex;
                
                // Handle optional texture
                if (vertexStream.peek() == ' ') vertexStream >> tIndex; // Read texture index if available

                // Convert OBJ 1-based index to 0-based index
                vIndex--;  // Convert to 0-based index
                if (tIndex > 0) tIndex--;  // Convert to 0-based index if texture exists

                // Add vertex position to vertices list
                vertices.push_back(tempVertices[vIndex].x);
                vertices.push_back(tempVertices[vIndex].y);
                vertices.push_back(tempVertices[vIndex].z);

                // Add texture coordinate to texture coordinates list
                if (tIndex > 0)
                {
                    texCoords.push_back(tempTexCoords[tIndex - 1].x);
                    texCoords.push_back(tempTexCoords[tIndex - 1].y);
                }
                else
                {
                    texCoords.push_back(0.0f);
                    texCoords.push_back(0.0f);
                }
            }
        }
    }

    return true;
}
