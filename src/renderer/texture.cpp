#include "texture.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// DDS Header structure
struct DDSHeader 
{
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitchOrLinearSize;
    uint32_t depth;
    uint32_t mipMapCount;
    uint32_t reserved1[11];
    struct {
        uint32_t size;
        uint32_t flags;
        char fourCC[4];
        uint32_t RGBBitCount;
        uint32_t RBitMask;
        uint32_t GBitMask;
        uint32_t BBitMask;
        uint32_t ABitMask;
    } pixelFormat;
    uint32_t caps;
    uint32_t caps2;
    uint32_t caps3;
    uint32_t caps4;
    uint32_t reserved2;
};

Texture::Texture() {}

bool Texture::load(const std::string &path) 
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open texture file: " << path << std::endl;
        return false;
    }

    // Read header
    char fileCode[4];
    file.read(fileCode, 4);
    if (std::memcmp(fileCode, "DDS ", 4) != 0)
    {
        file.close();
        std::cerr << "Invalid texture file: " << path << std::endl;
        return false;
    }

    // Read DDS header
    DDSHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(DDSHeader));

    // Check for DXT5 format
    if (std::memcmp(header.pixelFormat.fourCC, "DXT5", 4) != 0)
    {
        file.close();
        std::cerr << "Unsupported texture format: " << path << std::endl;
        return false;
    }

    // Calculate mipmap levels and total size
    uint32_t blockSize = 16; // DXT5 block size
    uint32_t width = header.width;
    uint32_t height = header.height;
    uint32_t mipMapCount = header.mipMapCount > 0 ? header.mipMapCount : 1;
    uint32_t bufferSize = 0;

    for (uint32_t i = 0; i < mipMapCount; ++i) 
    {
        bufferSize += ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        width = std::max(1U, width / 2);
        height = std::max(1U, height / 2);
    }

    // Read compressed data
    char* data = new char[bufferSize];
    file.read(data, bufferSize);
    file.close();

    // Create OpenGL texture
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load mipmaps
    width = header.width;
    height = header.height;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < mipMapCount; ++i) 
    {
        uint32_t size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, width, height, 0, size, data + offset);
        offset += size;
        width = std::max(1U, width / 2);
        height = std::max(1U, height / 2);
    }

    delete[] data;

    return true;
}

Texture::~Texture() 
{
    if (m_id) glDeleteTextures(1, &m_id);
}

GLuint Texture::getID() const
{
    return m_id;
}
