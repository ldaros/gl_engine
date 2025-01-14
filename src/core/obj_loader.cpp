#include "obj_loader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include <unordered_map>

// Structure to uniquely identify a vertex based on its indices
struct VertexIndex {
    unsigned int vertexIdx;
    unsigned int uvIdx;
    unsigned int normalIdx;

    // Equality operator for comparison in the unordered_map
    bool operator==(const VertexIndex &other) const {
        return vertexIdx == other.vertexIdx &&
               uvIdx == other.uvIdx &&
               normalIdx == other.normalIdx;
    }
};

// Hash function for VertexIndex to be used in unordered_map
struct VertexHash {
    std::size_t operator()(const VertexIndex& v) const {
        // Combine the hashes of the three indices
        return ((std::hash<unsigned int>()(v.vertexIdx) ^
                (std::hash<unsigned int>()(v.uvIdx) << 1)) >> 1) ^
                (std::hash<unsigned int>()(v.normalIdx) << 1);
    }
};

ObjLoader::ObjLoader() {}

ObjLoader::~ObjLoader() {}

bool ObjLoader::loadOBJ(
    const std::string &path, 
    std::vector<glm::vec3> &outVertices,
    std::vector<glm::vec2> &outUVs,
    std::vector<glm::vec3> &outNormals,
    std::vector<unsigned int> &outIndices // Added index output parameter
)
{
    // Temporary storage for parsing
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;

    // Map to keep track of unique vertices
    std::unordered_map<VertexIndex, unsigned int, VertexHash> uniqueVertices;

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
            std::string vertexStr;
            // OBJ files can have more than 3 vertices per face (e.g., quads), but here we assume triangles
            // If you need to support polygons, you'll need to implement a triangulation step
            while (lineStream >> vertexStr) {
                std::istringstream vertexStream(vertexStr);
                VertexIndex vi = {0, 0, 0};
                char slash;
                
                // Parse the vertex string (format: vertex/uv/normal)
                vertexStream >> vi.vertexIdx;
                if (vertexStream.peek() == '/') {
                    vertexStream.ignore(); // Ignore '/'
                    if (vertexStream.peek() != '/') {
                        vertexStream >> vi.uvIdx;
                    }
                    if (vertexStream.peek() == '/') {
                        vertexStream.ignore(); // Ignore '/'
                        vertexStream >> vi.normalIdx;
                    }
                }

                // Check if the vertex combination already exists
                if (uniqueVertices.find(vi) == uniqueVertices.end()) {
                    // If not, add it to the output lists
                    outVertices.push_back(tempVertices[vi.vertexIdx - 1]);
                    if (!tempUvs.empty()) {
                        outUVs.push_back(tempUvs[vi.uvIdx - 1]);
                    }
                    if (!tempNormals.empty()) {
                        outNormals.push_back(tempNormals[vi.normalIdx - 1]);
                    }
                    // Assign a new index
                    unsigned int newIndex = static_cast<unsigned int>(outVertices.size() - 1);
                    uniqueVertices[vi] = newIndex;
                    outIndices.push_back(newIndex);
                } else {
                    // If it exists, reuse the index
                    outIndices.push_back(uniqueVertices[vi]);
                }
            }
        } else {
            // Skip other lines (comments, empty lines, etc.)
            continue;
        }
    }

    file.close();

    return true;
}
