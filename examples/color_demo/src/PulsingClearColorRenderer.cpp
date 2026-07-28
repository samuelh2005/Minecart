#include "PulsingClearColorRenderer.hpp"

#include <cmath>
#include <numbers>

namespace {
float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
}  // namespace

PulsingClearColorRenderer::PulsingClearColorRenderer(SDL_FColor colorA, SDL_FColor colorB,
                                                       float periodSeconds)
    : m_colorA(colorA),
      m_colorB(colorB),
      m_periodSeconds(periodSeconds),
      m_startTicks(SDL_GetTicks()) {}

void PulsingClearColorRenderer::render(SDL_GPUCommandBuffer* commandBuffer,
                                        SDL_GPUTexture* swapchainTexture, Uint32 /*width*/,
                                        Uint32 /*height*/) {
    const float elapsedSeconds =
        static_cast<float>(SDL_GetTicks() - m_startTicks) / 1000.0f;
    const float phase =
        std::sin(elapsedSeconds * (2.0f * std::numbers::pi_v<float>) / m_periodSeconds) * 0.5f +
        0.5f;

    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = swapchainTexture;
    colorTarget.clear_color = SDL_FColor{
        lerp(m_colorA.r, m_colorB.r, phase),
        lerp(m_colorA.g, m_colorB.g, phase),
        lerp(m_colorA.b, m_colorB.b, phase),
        1.0f,
    };
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr);
    SDL_EndGPURenderPass(pass);
}
