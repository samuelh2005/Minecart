#include "engine/Window.hpp"

#include <stdexcept>
#include <utility>

Window::Window(SDL_GPUDevice* device, const std::string& title, int width, int height,
               std::unique_ptr<Renderer> renderer)
    : m_device(device), m_renderer(std::move(renderer)) {
    m_window = SDL_CreateWindow(title.c_str(), width, height, 0);
    if (!m_window) {
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    if (!SDL_ClaimWindowForGPUDevice(m_device, m_window)) {
        SDL_DestroyWindow(m_window);
        throw std::runtime_error(std::string("SDL_ClaimWindowForGPUDevice failed: ") +
                                  SDL_GetError());
    }
}

Window::~Window() {
    release();
}

Window::Window(Window&& other) noexcept
    : m_device(other.m_device),
      m_window(other.m_window),
      m_renderer(std::move(other.m_renderer)),
      m_wantsClose(other.m_wantsClose) {
    other.m_device = nullptr;
    other.m_window = nullptr;
}

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        release();
        m_device = other.m_device;
        m_window = other.m_window;
        m_renderer = std::move(other.m_renderer);
        m_wantsClose = other.m_wantsClose;
        other.m_device = nullptr;
        other.m_window = nullptr;
    }
    return *this;
}

void Window::release() {
    if (m_window) {
        if (m_device) {
            SDL_ReleaseWindowFromGPUDevice(m_device, m_window);
        }
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

SDL_WindowID Window::id() const {
    return SDL_GetWindowID(m_window);
}

void Window::render() {
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmd) {
        SDL_Log("SDL_AcquireGPUCommandBuffer failed: %s", SDL_GetError());
        return;
    }

    SDL_GPUTexture* swapchainTexture = nullptr;
    Uint32 width = 0;
    Uint32 height = 0;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, m_window, &swapchainTexture, &width,
                                                &height)) {
        SDL_Log("SDL_WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
        SDL_SubmitGPUCommandBuffer(cmd);
        return;
    }

    if (swapchainTexture == nullptr) {
        // Can happen e.g. when the window is minimized. Not an error, but the
        // command buffer must still be submitted.
        SDL_SubmitGPUCommandBuffer(cmd);
        return;
    }

    m_renderer->render(cmd, swapchainTexture, width, height);

    SDL_SubmitGPUCommandBuffer(cmd);
}
