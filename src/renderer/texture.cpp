#include "texture.h"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool initializeTexture(Texture& texture)
{
    // Create OpenGL texture if it doesn't exist
    if (!texture.glTextureId)
    {
        glGenTextures(1, &texture.glTextureId);
    }
    
    glBindTexture(GL_TEXTURE_2D, texture.glTextureId);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Determine format based on number of channels
    GLint internalFormat;
    GLenum format;
    switch (texture.channels) 
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
            std::cerr << "Unsupported number of channels: " << texture.channels << std::endl;
            return false;
    }

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 
                 texture.width, texture.height, 0,
                 format, GL_UNSIGNED_BYTE, 
                 texture.pixels.data());

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}

void bindTexture(const Texture&  texture, unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.glTextureId);
}

void cleanupTexture(Texture& texture)
{
    glDeleteTextures(1, &texture.glTextureId);
}
