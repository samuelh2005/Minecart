#include "ClearColorRenderer.hpp"

ClearColorRenderer::ClearColorRenderer(SDL_FColor color) : m_color(color) {}

void ClearColorRenderer::render(SDL_GPUCommandBuffer* commandBuffer,
                                 SDL_GPUTexture* swapchainTexture, Uint32 /*width*/,
                                 Uint32 /*height*/) {
    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = swapchainTexture;
    colorTarget.clear_color = m_color;
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr);
    SDL_EndGPURenderPass(pass);
}
