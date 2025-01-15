#include "model_loader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

ModelLoader::ModelLoader() {}

ModelLoader::~ModelLoader() {}

bool ModelLoader::load(const std::string& path, ModelLoader::MeshData& out_mesh)
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
        out_mesh.vertices.reserve(aiMesh->mNumVertices);
        out_mesh.normals.reserve(aiMesh->mNumVertices);
        if (aiMesh->mTextureCoords[0]) 
        {
            out_mesh.uvs.reserve(aiMesh->mNumVertices);
        }
        out_mesh.indices.reserve(aiMesh->mNumFaces * 3);

        // Process vertices
        for (unsigned int i = 0; i < aiMesh->mNumVertices; i++) 
        {
            // Vertices
            out_mesh.vertices.push_back(glm::vec3(
                aiMesh->mVertices[i].x,
                aiMesh->mVertices[i].y,
                aiMesh->mVertices[i].z
            ));

            // Normals
            if (aiMesh->mNormals) 
            {
                out_mesh.normals.push_back(glm::vec3(
                    aiMesh->mNormals[i].x,
                    aiMesh->mNormals[i].y,
                    aiMesh->mNormals[i].z
                ));
            }

            // UVs
            if (aiMesh->mTextureCoords[0])
            {
                out_mesh.uvs.push_back(glm::vec2(
                    aiMesh->mTextureCoords[0][i].x,
                    aiMesh->mTextureCoords[0][i].y
                ));
            }
        }

        // Process indices
        for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) 
        {
            const aiFace& face = aiMesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) 
            {
                out_mesh.indices.push_back(face.mIndices[j]);
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
