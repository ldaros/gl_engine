#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <GL/glew.h>

class RenderTarget 
{
public:
    RenderTarget(int width, int height);
    ~RenderTarget();

    void bind();
    void unbind();
    GLuint getTexture();
    GLuint getDepthTexture();

private:
    int m_width, m_height;
    GLuint m_framebuffer;
    GLuint m_texture, m_depthTexture;
    void init();
    void cleanup();
};

#endif // RENDER_TARGET_H