#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>


#include "core/resource_manager.h"

class Texture 
{
public:
    Texture();
    ~Texture();

    bool initialize(const TextureData& texture);
    void bind(unsigned int slot = 0) const;

    GLuint getID() const { return m_id; }
    bool isValid() const { return m_id != 0; }

private:
    GLuint m_id;
};

#endif // TEXTURE_H