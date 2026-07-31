#include "engine/Application.hpp"

#include <algorithm>
#include <stdexcept>

Application::Application() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    // One GPU device shared by every window (SDL_GPU's intended usage
    // pattern). Ask for whichever shader format the platform's chosen
    // backend needs; SDL_CreateGPUDevice picks the backend for us.
    m_device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        true,      // enable debug layers in development builds
        nullptr);  // let SDL choose the best available backend

    if (!m_device) {
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateGPUDevice failed: ") + SDL_GetError());
    }
}

Application::~Application() {
    // Windows must release themselves from the GPU device before the device
    // itself is destroyed, so clear them explicitly first.
    m_windows.clear();

    if (m_device) {
        SDL_DestroyGPUDevice(m_device);
    }

    SDL_Quit();
}

SDL_WindowID Application::createWindow(const std::string& title, int width, int height,
                                        std::shared_ptr<Renderer> renderer) {
    auto window = std::make_unique<Window>(m_device, title, width, height, std::move(renderer));
    const SDL_WindowID id = window->id();
    m_windows.push_back(std::move(window));
    return id;
}

void Application::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_QUIT:
            m_running = false;
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
            const SDL_WindowID id = event.window.windowID;
            auto it = std::find_if(m_windows.begin(), m_windows.end(),
                                    [id](const std::unique_ptr<Window>& w) {
                                        return w->id() == id;
                                    });
            if (it != m_windows.end()) {
                (*it)->requestClose();
            }
            break;
        }

        default:
            break;
    }
}

void Application::run() {
    // onInit() is called here rather than from the constructor: virtual
    // dispatch to a derived override doesn't work until the derived object
    // is fully constructed, so the base class constructor is the wrong
    // place for it.
    onInit();

    Uint64 lastTicks = SDL_GetTicks();

    while (m_running && !m_windows.empty()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handleEvent(event);
            onEvent(event);
        }

        // Drop any windows that were asked to close this frame.
        m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(),
                                        [](const std::unique_ptr<Window>& w) {
                                            return w->wantsClose();
                                        }),
                         m_windows.end());

        if (m_windows.empty()) {
            m_running = false;
            break;
        }

        const Uint64 nowTicks = SDL_GetTicks();
        const float deltaTime = static_cast<float>(nowTicks - lastTicks) / 1000.0f;
        lastTicks = nowTicks;

        onTick(deltaTime);

        for (auto& window : m_windows) {
            window->render();
        }
    }
}
