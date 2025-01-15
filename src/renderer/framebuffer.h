#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>

class Framebuffer 
{
public:
    Framebuffer(int width, int height, bool depth = true, bool color = true);
    ~Framebuffer();

    bool init();
    void bind();
    void unbind();
    GLuint getTexture() const { return m_texture; }
    GLuint getDepthTexture() const { return m_depthTexture; }

private:
    void cleanup();
    
    int m_width, m_height;
    bool m_depth, m_color;
    GLuint m_framebuffer;
    GLuint m_texture, m_depthTexture;
};

#endif // FRAMEBUFFER_H