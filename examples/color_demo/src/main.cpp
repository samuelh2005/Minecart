#include <SDL3/SDL.h>

#include <exception>

#include "ColorDemoApplication.hpp"

int main(int /*argc*/, char** /*argv*/) {
    try {
        // ColorDemoApplication owns deciding which windows exist and how
        // they're rendered (via onInit()); main only owns the process
        // lifetime.
        ColorDemoApplication app;
        app.run();
    } catch (const std::exception& e) {
        SDL_Log("Fatal error: %s", e.what());
        return 1;
    }

    return 0;
}
