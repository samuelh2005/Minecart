#pragma once

#include <SDL3/SDL.h>

#include <memory>
#include <string>
#include <vector>

#include "engine/Renderer.hpp"
#include "engine/Window.hpp"

// Owns SDL's lifetime, the single shared SDL_GPUDevice, and the collection of
// live Windows. This is engine-level plumbing only: *which* windows exist and
// *how* the app reacts to events or time is entirely up to a concrete
// subclass, expressed through three hooks:
//
//   - onInit()             called once, before the loop starts. Concrete
//                           applications create their initial windows here.
//   - onEvent(event)        called for every SDL event, after this class's
//                           own built-in handling (quit / close-requested).
//   - onTick(deltaTime)      called once per frame with the elapsed time in
//                           seconds since the previous tick.
class Application {
public:
    Application();
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Calls onInit(), then runs the event/render loop until every window has
    // been closed or the app receives a quit event. Each iteration polls SDL
    // events (handling SDL_EVENT_QUIT / SDL_EVENT_WINDOW_CLOSE_REQUESTED
    // internally, then forwarding every event to onEvent()), prunes windows
    // that were asked to close, calls onTick(deltaTime), and renders whatever
    // windows remain.
    void run();

protected:
    // Called once, before the loop starts, so a concrete Application can set
    // up its initial windows and Renderers.
    virtual void onInit() = 0;

    // Called for every SDL event polled during the loop, after this class's
    // own handling has run. Default implementation does nothing.
    virtual void onEvent(const SDL_Event& event) {}

    // Called once per frame, after event handling and after closed windows
    // have been pruned. deltaTime is the elapsed time in seconds since the
    // previous tick. Default implementation does nothing.
    virtual void onTick(float deltaTime) {}

    // Creates a new top-level window using the given Renderer and returns
    // its SDL_WindowID. Exposed to derived classes so they can build their
    // windows from onInit().
    SDL_WindowID createWindow(const std::string& title, int width, int height,
                               std::shared_ptr<Renderer> renderer);

    // The shared GPU device, in case a derived class needs it directly (e.g.
    // to create GPU resources beyond what Window itself manages).
    [[nodiscard]] SDL_GPUDevice* device() const { return m_device; }

private:
    void handleEvent(const SDL_Event& event);

    SDL_GPUDevice* m_device = nullptr;
    std::vector<std::unique_ptr<Window>> m_windows;
    bool m_running = true;
};
