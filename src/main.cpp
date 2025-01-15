#include "core/application.h"

int main() {
    Application app;
    
    if (!app.initialize(1200, 900, "OG Engine")) {
        return EXIT_FAILURE;
    }
    
    app.run();
    app.cleanup();
    
    return EXIT_SUCCESS;
}