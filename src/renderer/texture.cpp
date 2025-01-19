#include "texture.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Texture::Texture(): m_id(0) {}

Texture::~Texture() 
{
    if (m_id != 0)
    {
        glDeleteTextures(1, &m_id);
    }
}

bool Texture::initialize(const TextureData &image)
{
    // Create OpenGL texture if it doesn't exist
    if (!m_id) 
    {
        glGenTextures(1, &m_id);
    }
    
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine format based on number of channels
    GLint internalFormat;
    GLenum format;
    switch (image.channels) 
    {
        case 1:
            internalFormat = GL_R8;
            format = GL_RED;
            break;
        case 2:
            internalFormat = GL_RG8;
            format = GL_RG;
            break;
        case 3:
            internalFormat = GL_RGB8;
            format = GL_RGB;
            break;
        case 4:
            internalFormat = GL_RGBA8;
            format = GL_RGBA;
            break;
        default:
            std::cerr << "Unsupported number of channels: " << image.channels << std::endl;
            return false;
    }

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 
                 image.width, image.height, 0,
                 format, GL_UNSIGNED_BYTE, 
                 image.pixels.data());

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}

void Texture::bind(unsigned int slot) const 
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
