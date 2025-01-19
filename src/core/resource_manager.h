#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
 
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct TextureData 
{
    std::vector<unsigned char> pixels;
    int width;
    int height;
    int channels;

    bool hasAlpha() const { return channels == 4; }
    size_t getSizeInBytes() const { return pixels.size(); }
    bool isEmpty() const { return pixels.empty(); }
};

struct MeshData
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<unsigned int> indices;
};

struct ShaderData 
{
    std::string vertexCode;
    std::string fragmentCode;
    unsigned int programId{0};
};

class ResourceManager
{
public:
    static ResourceManager& getInstance() 
    {
        static ResourceManager instance;
        return instance;
    }

    std::shared_ptr<TextureData> loadTexture(const std::string& path);
    std::shared_ptr<MeshData> loadMesh(const std::string& path);
    std::shared_ptr<ShaderData> loadShader(const std::string& vertexPath, const std::string& fragmentPath);

    void cleanup();

private:
    ResourceManager() = default;

    std::string resolvePath(const std::string& path);
    bool loadTextureFromFile(const std::string& path, TextureData* texture);
    bool loadMeshFromFile(const std::string& path, MeshData* mesh);
    bool loadShaderFromFiles(const std::string& vertexPath, const std::string& fragmentPath, ShaderData* shader);

    std::map<std::string, std::shared_ptr<TextureData>> m_textures;
    std::map<std::string, std::shared_ptr<MeshData>> m_meshes;
    std::map<std::string, std::shared_ptr<ShaderData>> m_shaders;
};

#endif // RESOURCE_MANAGER_H