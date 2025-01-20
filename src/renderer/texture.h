#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include "core/resource_manager.h"

bool initializeTexture(Texture& texture);
void bindTexture(const Texture&  texture, unsigned int slot = 0);
void cleanupTexture(Texture& texture);

#endif // TEXTURE_H