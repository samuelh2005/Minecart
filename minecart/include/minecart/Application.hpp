#pragma once

#include "minecart/resource/ResourceManager.hpp"
#include "minecart/graphics/Window.hpp"
#include "minecart/Localisation.hpp"

namespace minecart {

class Application {
public:
    virtual ~Application() = default;

    void run();
    void stop();

    resource::ResourceManager& GetResourceManager() { return *resourceManager_; }
    Localisation& GetLocalisation() { return *localisation_; }

protected:
    virtual void onInit() = 0;
    virtual void onShutdown() = 0;
    virtual void onUpdate(double dt) = 0;
    virtual void postUpdate() {}

    bool isRunning() const noexcept { return running_; }

private:
    bool running_ = true;
    resource::ResourceManager* resourceManager_ = nullptr;
    Localisation* localisation_ = nullptr;
};

struct HeadlessConfig {
    int tickRate;
    const char* assetsPath;
};

class HeadlessEntrypoint : public Application {
public:
    explicit HeadlessEntrypoint(HeadlessConfig config);

protected:
    void onInit() override;
    void onShutdown() override;
    void onUpdate(double dt) override;

    virtual void onServerInit() = 0;
    virtual void onServerShutdown() = 0;
    virtual void onServerUpdate(double dt) = 0;

private:
    HeadlessConfig config_;
};

struct GraphicalConfig {
    int targetFps;
    bool vsync;
    bool fullscreen;
    int windowWidth;
    int windowHeight;
    const char* windowTitle;
    const char* assetsPath;
};

class GraphicalEntrypoint : public Application {
public:
    explicit GraphicalEntrypoint(GraphicalConfig config);
    Window* GetWindow() { return window_; }

protected:
    void onInit() override;
    void onShutdown() override;
    void onUpdate(double dt) override;
    void postUpdate() override;

    virtual void onClientInit() = 0;
    virtual void onClientShutdown() = 0;
    virtual void onClientUpdate(double dt) = 0;
    virtual void onClientEvent(const SDL_Event& event) = 0;
    virtual void onClientRender(double dt) = 0;

private:
    GraphicalConfig config_;
    Window* window_ = nullptr;
    std::uint64_t lastCounter;
    std::uint64_t frequency;
};

} // namespace minecart