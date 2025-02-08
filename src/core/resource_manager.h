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
    ResourceManager() = default;

    std::shared_ptr<Image> loadTexture(const std::string& path);
    std::shared_ptr<MeshData> loadMesh(const std::string& path);
    std::shared_ptr<Material> loadMaterial(const std::string& path);

    void cleanup();

private:
    bool loadTextureFromFile(const std::string& path, Image* texture);
    bool loadMeshFromFile(const std::string& path, MeshData* mesh);
    bool deserializeMaterial(const std::string& path, Material* material);

    std::map<UUID, std::shared_ptr<Image>> m_textures;
    std::map<UUID, std::shared_ptr<MeshData>> m_meshes;
    std::map<UUID, std::shared_ptr<Material>> m_materials;
};

}