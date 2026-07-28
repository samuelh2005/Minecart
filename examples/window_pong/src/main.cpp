#include <SDL3/SDL.h>

#include <exception>

#include "WindowPongApplication.hpp"

int main(int /*argc*/, char** /*argv*/) {
    try {
        WindowPongApplication app;
        app.run();
    } catch (const std::exception& e) {
        SDL_Log("Fatal error: %s", e.what());
        return 1;
    }

    return 0;
}
