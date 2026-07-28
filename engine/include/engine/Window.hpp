#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>

#include "engine/Renderer.hpp"

// Wraps a single SDL_Window claimed for the shared SDL_GPUDevice. A Window
// no longer knows *how* to render -- it only knows the mechanics of getting
// a frame onto the screen (acquire command buffer, acquire swapchain
// texture, submit) and delegates the actual render-pass content to whatever
// Renderer it was given. This lets two Windows use the same Renderer
// subclass (identical or parameterized differently) or entirely different
// Renderer subclasses, with no changes to Window itself.
class Window {
public:
    Window(SDL_GPUDevice* device, const std::string& title, int width, int height,
           std::unique_ptr<Renderer> renderer);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    // Acquires this window's swapchain texture for the current frame and
    // hands it to the Renderer to record a render pass into.
    void render();

    [[nodiscard]] SDL_WindowID id() const;
    [[nodiscard]] bool wantsClose() const { return m_wantsClose; }
    void requestClose() { m_wantsClose = true; }

private:
    void release();

    SDL_GPUDevice* m_device = nullptr;
    SDL_Window* m_window = nullptr;
    std::unique_ptr<Renderer> m_renderer;
    bool m_wantsClose = false;
};
