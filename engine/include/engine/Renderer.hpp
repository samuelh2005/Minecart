#pragma once

#include <SDL3/SDL.h>

// Abstract interface for "how a frame gets rendered", decoupled from
// "what a window is". A Window owns exactly one Renderer and delegates the
// actual render pass to it every frame.
//
// Because this is an abstract base, an Application is free to hand out:
//   - two windows using the *same* Renderer subclass (optionally with
//     different constructor parameters, e.g. two ClearColorRenderers with
//     different colors), or
//   - two windows using *different* Renderer subclasses entirely (e.g. one
//     window that clears to a solid color and another that animates a
//     gradient), or anything else that can be expressed as GPU render-pass
//     work, without Window ever needing to change.
class Renderer {
public:
    virtual ~Renderer() = default;

    // Called once per frame by the owning Window, after it has acquired a
    // command buffer and this frame's swapchain texture. Implementations own
    // the render pass: begin it, record whatever GPU work they want, and end
    // it. The Window is responsible for acquiring/submitting the command
    // buffer; this method only fills in what happens in between.
    virtual void render(SDL_GPUCommandBuffer* commandBuffer, SDL_GPUTexture* swapchainTexture,
                         Uint32 width, Uint32 height) = 0;
};
