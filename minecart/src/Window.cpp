#include "minecart/graphics/Window.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include <spdlog/spdlog.h>

namespace minecart {

Window::~Window() {
    shutdown();
}

bool Window::initialize(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        spdlog::error("SDL_Init failed: {}", SDL_GetError());
        return false;
    }
    m_sdlInitialized = true;

    SDL_WindowFlags windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    m_window = SDL_CreateWindow(title, width, height, windowFlags);
    if (!m_window) {
        spdlog::error("SDL_CreateWindow failed: {}", SDL_GetError());
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_renderer) {
        spdlog::error("SDL_CreateRenderer failed: {}", SDL_GetError());
        return false;
    }

    SDL_SetRenderVSync(m_renderer, 1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer)) {
        spdlog::error("ImGui_ImplSDL3_InitForSDLRenderer failed");
        return false;
    }
    if (!ImGui_ImplSDLRenderer3_Init(m_renderer)) {
        spdlog::error("ImGui_ImplSDLRenderer3_Init failed");
        return false;
    }

    m_imguiInitialized = true;
    return true;
}

void Window::shutdown() {
    if (m_imguiInitialized) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        m_imguiInitialized = false;
    }

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    if (m_sdlInitialized) {
        SDL_Quit();
        m_sdlInitialized = false;
    }
}

void Window::beginFrame() {
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui::NewFrame();
}

void Window::endFrame() {
    ImGui::Render();
    SDL_SetRenderDrawColor(m_renderer, 20, 20, 20, 255);
    SDL_RenderClear(m_renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);
    SDL_RenderPresent(m_renderer);
}

} // namespace minecart
