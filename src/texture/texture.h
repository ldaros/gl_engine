#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

class Texture 
{
public:
    Texture(const std::string &path);
    ~Texture();

    GLuint getID() const;

private:
    GLuint id;
};

#endif // TEXTURE_H