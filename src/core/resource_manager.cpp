 #include "resource_manager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

namespace Engine {

std::shared_ptr<Image> ResourceManager::loadTexture(const std::string& path)
{   
    Image texture;
    if (!loadTextureFromFile(path, &texture)) 
    {
        return nullptr;
    }

    UUID uuid = UUID_generate();
    texture.uuid = uuid;

    std::shared_ptr<Image> sharedTexture = std::make_shared<Image>(texture);
    m_textures[uuid] = sharedTexture;
    return sharedTexture;
}

std::shared_ptr<MeshData> ResourceManager::loadMesh(const std::string& path)
{
    MeshData mesh;
    if (!loadMeshFromFile(path, &mesh)) 
    {
        return nullptr;
    }

    UUID uuid = UUID_generate();
    mesh.uuid = uuid;

    std::shared_ptr<MeshData> sharedMesh = std::make_shared<MeshData>(mesh);
    m_meshes[uuid] = sharedMesh;
    return sharedMesh;
}

void ResourceManager::cleanup() 
{
    m_textures.clear();
    m_meshes.clear();
}

bool ResourceManager::loadTextureFromFile(const std::string& path, Image* texture) 
{
    // Clear any existing data
    texture->pixels.clear();
    texture->width = 0;
    texture->height = 0;
    texture->channels = 0;

    // Load the image using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);  // Flip images for OpenGL
    
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (!data) 
    {
        std::cerr << "Failed to load image: " << path << "\n";
        std::cerr << "STB Error: " << stbi_failure_reason() << "\n";
        return false;
    }

    // Set the image properties
    texture->width = width;
    texture->height = height;
    texture->channels = channels;

    // Copy the pixel data
    size_t dataSize = width * height * channels;
    texture->pixels.resize(dataSize);
    std::memcpy(texture->pixels.data(), data, dataSize);

    // Free the stb_image data
    stbi_image_free(data);

    return true;
}

bool ResourceManager::loadMeshFromFile(const std::string& path, MeshData* mesh)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    if (scene->mNumMeshes > 0) 
    {
        aiMesh* aiMesh = scene->mMeshes[0];

        // Reserve space
        mesh->vertices.reserve(aiMesh->mNumVertices);
        mesh->normals.reserve(aiMesh->mNumVertices);
        if (aiMesh->mTextureCoords[0]) 
        {
            mesh->uvs.reserve(aiMesh->mNumVertices);
        }
        mesh->indices.reserve(aiMesh->mNumFaces * 3);

        // Process vertices
        for (uint32_t i = 0; i < aiMesh->mNumVertices; i++) 
        {
            // Vertices
            mesh->vertices.push_back(glm::vec3(
                aiMesh->mVertices[i].x,
                aiMesh->mVertices[i].y,
                aiMesh->mVertices[i].z
            ));

            // Normals
            if (aiMesh->mNormals) 
            {
                mesh->normals.push_back(glm::vec3(
                    aiMesh->mNormals[i].x,
                    aiMesh->mNormals[i].y,
                    aiMesh->mNormals[i].z
                ));
            }

            // UVs
            if (aiMesh->mTextureCoords[0])
            {
                mesh->uvs.push_back(glm::vec2(
                    aiMesh->mTextureCoords[0][i].x,
                    aiMesh->mTextureCoords[0][i].y
                ));
            }

            // Tangents
            if (aiMesh->mTangents) 
            {
                mesh->tangents.push_back(glm::vec3(
                    aiMesh->mTangents[i].x,
                    aiMesh->mTangents[i].y,
                    aiMesh->mTangents[i].z
                ));
            }

            // Bitangents
            if (aiMesh->mBitangents) 
            {
                mesh->bitangents.push_back(glm::vec3(
                    aiMesh->mBitangents[i].x,
                    aiMesh->mBitangents[i].y,
                    aiMesh->mBitangents[i].z
                ));
            }
        }

        // Process indices
        for (uint32_t i = 0; i < aiMesh->mNumFaces; i++) 
        {
            const aiFace& face = aiMesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) 
            {
                mesh->indices.push_back(face.mIndices[j]);
            }
        }
    }
    else 
    {
        std::cerr << "No meshes found." << std::endl;
        return false;
    }

    return true;
}

}