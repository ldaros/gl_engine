#include "core/application.h"

int main() 
{
    Engine::Application app;
    
    if (!app.initialize(1920, 1080, "OG Engine")) 
    {
        return EXIT_FAILURE;
    }
    if (!app.loadScene("resources/scenes/test_scene.json")) 
    {
        return EXIT_FAILURE;
    }

    app.run();
    app.cleanup();
    
    return EXIT_SUCCESS;
}