#pragma once

#include <map>
#include <string>
#include <memory>

#include "uuid.h"
#include "resources.h"

namespace Engine {

class ResourceManager
{
public:
    static ResourceManager& getInstance() 
    {
        static ResourceManager instance;
        return instance;
    }

    std::shared_ptr<Image> loadTexture(const std::string& path);
    std::shared_ptr<MeshData> loadMesh(const std::string& path);

    void cleanup();

private:
    ResourceManager() = default;

    std::string resolvePath(const std::string& path);
    bool loadTextureFromFile(const std::string& path, Image* texture);
    bool loadMeshFromFile(const std::string& path, MeshData* mesh);

    std::map<UUID, std::shared_ptr<Image>> m_textures;
    std::map<UUID, std::shared_ptr<MeshData>> m_meshes;
};

}