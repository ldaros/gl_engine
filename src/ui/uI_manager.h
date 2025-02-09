#pragma once

#include "core/window.h"
#include "scene/scene.h"

namespace Engine {

class UIManager 
{
    public:
    void initialize(Window& window);
    void cleanup();
    
    void startFrame();
    void endFrame();

private:
    void SetupImGuiStyle();
};

}