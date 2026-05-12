#include "minecart/Application.hpp"
#include "minecart/resource/ResourceLoader.hpp"

#include <chrono>
#include <thread>
#include <filesystem>
#include <imgui_impl_sdl3.h>

namespace minecart
{

namespace fs = std::filesystem;

// -------------------- Application --------------------

void Application::run() {
    this->resourceManager_ = new resource::ResourceManager();
    this->localisation_ = new Localisation();

    auto lang = std::make_unique<minecart::resource::Registry<minecart::TranslationDefinition>>(
        "lang",
        [](const minecart::resource::ResourceKey& key, const fs::path& file) {
            return minecart::resource::loadJsonResource<minecart::TranslationDefinition>(key, file);
        }
    );
    this->resourceManager_->addRegistry(std::move(lang));

    this->localisation_->init(*this->resourceManager_);
    this->localisation_->SetLanguage("en_us");

    onInit();

    auto lastTime = std::chrono::high_resolution_clock::now();

    constexpr double fixedDt = 1.0 / 60.0;
    double accumulator = 0.0;

    while (running_) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - lastTime;
        lastTime = now;

        accumulator += elapsed.count();

        while (accumulator >= fixedDt) {
            onUpdate(fixedDt);
            accumulator -= fixedDt;
        }

        postUpdate();
    }

    onShutdown();
}

void Application::stop() {
    running_ = false;
}

// -------------------- HeadlessEntrypoint --------------------

HeadlessEntrypoint::HeadlessEntrypoint(HeadlessConfig config)
    : config_(config)
{
}

void HeadlessEntrypoint::onInit() {
    onServerInit();

    // load packs after user init, so that they can add their own registries if needed
    minecart::resource::ResourceLoader loader(this->GetResourceManager());
    loader.loadPack(config_.assetsPath);
}

void HeadlessEntrypoint::onShutdown() {
    onServerShutdown();
}

void HeadlessEntrypoint::onUpdate(double dt) {
    onServerUpdate(dt);
}

// -------------------- GraphicalEntrypoint --------------------

GraphicalEntrypoint::GraphicalEntrypoint(GraphicalConfig config)
    : config_(config)
{
}

void GraphicalEntrypoint::onInit() {
    this->window_ = new Window();
    if (!this->window_->initialize(config_.windowTitle, config_.windowWidth, config_.windowHeight)) {
        spdlog::error("Failed to initialize window");
        std::exit(1);
    }

    this->lastCounter = SDL_GetPerformanceCounter();
    this->frequency = SDL_GetPerformanceFrequency();

    onClientInit();

    // load packs after user init, so that they can add their own registries if needed
    minecart::resource::ResourceLoader loader(this->GetResourceManager());
    loader.loadPack(config_.assetsPath);
}

void GraphicalEntrypoint::postUpdate() {
    if (window_ && this->isRunning()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            onClientEvent(event);
        }

        const std::uint64_t currentCounter = SDL_GetPerformanceCounter();
        const float deltaSeconds = static_cast<float>(currentCounter - lastCounter) / static_cast<float>(frequency);
        lastCounter = currentCounter;

        window_->beginFrame();
        onClientRender(deltaSeconds);
        window_->endFrame();
    }
}

void GraphicalEntrypoint::onShutdown() {
    onClientShutdown();
    if (window_) {
        window_->shutdown();
        delete window_;
        window_ = nullptr;
    }
}

void GraphicalEntrypoint::onUpdate(double dt) {
    onClientUpdate(dt);
}


} // namespace minecart