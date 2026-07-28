#pragma once

#include "engine/Renderer.hpp"

// A second, entirely different Renderer implementation: instead of a fixed
// clear color, it animates (lerps) between two colors over time. Exists to
// demonstrate that Renderer is a real extension point -- a Window using this
// class needs no special-casing anywhere else in the codebase.
class PulsingClearColorRenderer final : public Renderer {
public:
    PulsingClearColorRenderer(SDL_FColor colorA, SDL_FColor colorB, float periodSeconds = 2.0f);

    void render(SDL_GPUCommandBuffer* commandBuffer, SDL_GPUTexture* swapchainTexture,
                Uint32 width, Uint32 height) override;

private:
    SDL_FColor m_colorA;
    SDL_FColor m_colorB;
    float m_periodSeconds;
    Uint64 m_startTicks;
};
