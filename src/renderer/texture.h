#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture 
{
public:
    Texture();
    ~Texture();

    bool load(const std::string &path);
    GLuint getID() const;

private:
    GLuint m_id;
};

#endif // TEXTURE_H