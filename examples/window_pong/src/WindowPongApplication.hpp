#pragma once

#include "engine/Application.hpp"

// Concrete Application for this demo. All it does is decide, in onInit(),
// which windows exist and what each one renders with -- it has no per-frame
// or per-event behavior of its own, so onEvent()/onTick() are left at the
// engine's no-op defaults.
class WindowPongApplication final : public Application {
protected:
    void onInit() override;
    void onEvent(const SDL_Event& event) override;
    void onTick(float deltaTime) override;

private:
    SDL_WindowID m_player1Window;
    SDL_WindowID m_player2Window;
    SDL_WindowID m_ballWindow;

    // Player 1 window velocity, in pixels per second. Positive is down, negative is up.
    float m_player1Velocity = 0.0f;
    // Player 2 window velocity, in pixels per second. Positive is down, negative is up.
    float m_player2Velocity = 0.0f;
    // Ball window velocity, in pixels per second. Positive is down, negative is up.
    // Give the ball an initial left velocity of -300 pixels per second.
    float m_ballVelocityX = -300.0f;
    float m_ballVelocityY = 0.0f;
};
