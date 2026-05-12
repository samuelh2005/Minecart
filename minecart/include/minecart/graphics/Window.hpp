#pragma once

#include <SDL3/SDL.h>

namespace minecart {

class Window {
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool initialize(const char* title, int width, int height);
    void shutdown();

    [[nodiscard]] SDL_Window* sdlWindow() const noexcept { return m_window; }
    [[nodiscard]] SDL_Renderer* renderer() const noexcept { return m_renderer; }

    void beginFrame();
    void endFrame();

private:
    bool m_sdlInitialized = false;
    bool m_imguiInitialized = false;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};

} // namespace minecart
