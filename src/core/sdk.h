#pragma once

#include "window.h"
#include "renderer/opengl.h"
#include "scene/scene.h"
#include "ui/ui_manager.h"
#include "renderer/opengl.h"

namespace Engine {

struct SDK
{
    std::unique_ptr<Window> window;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<UIManager> uiManager;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<OpenGL::Renderer> renderer;
};

}