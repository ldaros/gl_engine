#include "core/application.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    Engine::Application app;
    std::string scenePath;

    for (int i = 1; i < argc; ++i) 
    {
        if (std::string(argv[i]) == "--scene") 
        {
          if (i + 1 < argc) 
          {
            scenePath = argv[i + 1];
            ++i;
          } 
          else 
          {
            std::cerr << "Error: --scene option requires a scene path." << std::endl;
            return EXIT_FAILURE;
          }
        }
      }
    
    if (!app.initialize(1920, 1080, "gl Engine")) 
    {
        return EXIT_FAILURE;
    }
    
    if (!app.loadScene(scenePath)) 
    {
        return EXIT_FAILURE;
    }

    app.run();
    app.cleanup();
    
    return EXIT_SUCCESS;
}