#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

#include "core/image_loader.h"

class Texture 
{
public:
    Texture();
    ~Texture();

    bool initialize(ImageLoader::ImageData& image);
    GLuint getID() const { return m_id; }

private:
    GLuint m_id;
};

#endif // TEXTURE_H