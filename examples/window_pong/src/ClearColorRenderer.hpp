#pragma once

#include "engine/Renderer.hpp"

// The simplest possible Renderer: clears the whole frame to a fixed color
// via a single SDL_GPU render pass with no draw calls.
class ClearColorRenderer final : public Renderer {
public:
    explicit ClearColorRenderer(SDL_FColor color);

    void render(SDL_GPUCommandBuffer* commandBuffer, SDL_GPUTexture* swapchainTexture,
                Uint32 width, Uint32 height) override;

    void setColor(SDL_FColor color) { m_color = color; }

private:
    SDL_FColor m_color;
};
