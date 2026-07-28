#include "ColorDemoApplication.hpp"

#include <memory>

#include "ClearColorRenderer.hpp"
#include "PulsingClearColorRenderer.hpp"

void ColorDemoApplication::onInit() {
    // Windows A and B both use ClearColorRenderer -- the same Renderer
    // subclass, just parameterized with different colors.
    createWindow("Window A - Red", 640, 480,
                 std::make_unique<ClearColorRenderer>(SDL_FColor{0.60f, 0.10f, 0.10f, 1.0f}));
    createWindow("Window B - Green", 640, 480,
                 std::make_unique<ClearColorRenderer>(SDL_FColor{0.10f, 0.60f, 0.10f, 1.0f}));

    // Window C uses a completely different Renderer subclass, proving the
    // abstraction supports windows that render in fundamentally different
    // ways with no changes to Window or Application's structure.
    createWindow("Window C - Pulsing", 640, 480,
                 std::make_unique<PulsingClearColorRenderer>(
                     SDL_FColor{0.05f, 0.05f, 0.40f, 1.0f}, SDL_FColor{0.90f, 0.80f, 0.10f, 1.0f}));
}
