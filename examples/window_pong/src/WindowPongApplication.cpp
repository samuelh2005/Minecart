#include "WindowPongApplication.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <memory>

#include "ClearColorRenderer.hpp"

void WindowPongApplication::onInit() {
    // 1. Get the SDL3 screen size and create a window for each player and the ball

    SDL_DisplayID display = SDL_GetPrimaryDisplay();

    if (display == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get primary display: %s", SDL_GetError());
        return;
    }

    SDL_Rect usable = {0, 0, 0, 0};
    SDL_GetDisplayUsableBounds(display, &usable);

    // 2. Create 3 windows, one for each player and one for the ball, and position them on the screen
    // Player 1 should be on the left, player 2 on the right, and the ball in the middle
    // Each player window should fill 60% of the entire height of the screen, and be 1/8 of the width of the screen
    // The ball window should be a square in the middle of the screen, with a size of 1/8 of the height of the screen

    SDL_Rect player1Rect = {usable.x, usable.y + usable.h / 5, usable.w / 8, usable.h * 3 / 5};
    SDL_Rect player2Rect = {usable.x + usable.w * 7 / 8, usable.y + usable.h / 5, usable.w / 8, usable.h * 3 / 5};
    SDL_Rect ballRect = {usable.x + usable.w / 2 - usable.h / 16, usable.y + usable.h / 2 - usable.h / 16, usable.h / 8, usable.h / 8};

    m_player1Window = createWindow(
        "Player 1",
        player1Rect.w, player1Rect.h,
        std::make_shared<ClearColorRenderer>(SDL_FColor{0.60f, 0.10f, 0.10f, 1.0f})
    );
    SDL_SetWindowPosition(SDL_GetWindowFromID(m_player1Window), player1Rect.x, player1Rect.y);

    m_player2Window = createWindow(
        "Player 2",
        player2Rect.w, player2Rect.h,
        std::make_shared<ClearColorRenderer>(SDL_FColor{0.10f, 0.60f, 0.10f, 1.0f})
    );
    SDL_SetWindowPosition(SDL_GetWindowFromID(m_player2Window), player2Rect.x, player2Rect.y);

    m_ballWindow = createWindow(
        "Ball",
        ballRect.w, ballRect.h,
        std::make_shared<ClearColorRenderer>(SDL_FColor{1.1f, 1.1f, 1.0f, 1.0f})
    );
    SDL_SetWindowPosition(SDL_GetWindowFromID(m_ballWindow), ballRect.x, ballRect.y);
}

void WindowPongApplication::onEvent(const SDL_Event& event) {
    // 1. Move the player 1 window up and down with the W and S keys
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        const SDL_KeyboardEvent& keyEvent = event.key;
        if (keyEvent.key == SDLK_W && keyEvent.down) {
            // Move player 1 window up
            m_player1Velocity = -500.0f;
        } else if (keyEvent.key == SDLK_S && keyEvent.down) {
            // Move player 1 window down
            m_player1Velocity = 500.0f;
        } else {
            m_player1Velocity = 0.0f;
        }
    }
    
    // 2. and move the player 2 window up and down with the Up and Down arrow keys
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        const SDL_KeyboardEvent& keyEvent = event.key;
        if (keyEvent.key == SDLK_UP && keyEvent.down) {
            // Move player 2 window up
            m_player2Velocity = -500.0f;
        } else if (keyEvent.key == SDLK_DOWN && keyEvent.down) {
            // Move player 2 window down
            m_player2Velocity = 500.0f;
        } else {
            m_player2Velocity = 0.0f;
        }
    }

    // Q: Why do we not filter events by Window IDs?
    // A: If we filtered by Window IDs it would require the player to click on each window before they could control it,
    // which is not a good user experience. Instead, we allow the player to control both windows with the same keyboard,
    // regardless of which window has focus. Even more so, the player could entirely miss their hit, as they are wasting
    // their time trying to click on the window instead of focusing on the game.
    //
    // Other games might want to filter events by Window IDs as they have a real reason to do so, but for pong, we want
    // to make it as easy as possible for the player to control both paddles with the same keyboard, so we do not filter
    // by Window IDs.
}

void WindowPongApplication::onTick(float deltaTime) {
    // Query the current display bounds once; used for wall and out-of-bounds checks.
    SDL_DisplayID display = SDL_GetPrimaryDisplay();
    SDL_Rect usable = {0, 0, 0, 0};
    SDL_GetDisplayUsableBounds(display, &usable);

    SDL_Window* player1Window = SDL_GetWindowFromID(m_player1Window);
    SDL_Window* player2Window = SDL_GetWindowFromID(m_player2Window);
    SDL_Window* ballWindow    = SDL_GetWindowFromID(m_ballWindow);

    // 1. Move the player windows based on velocity.
    if (m_player1Velocity != 0.0f) {
        int x, y;
        SDL_GetWindowPosition(player1Window, &x, &y);
        SDL_SetWindowPosition(player1Window, x, y + static_cast<int>(m_player1Velocity * deltaTime));
    }
    if (m_player2Velocity != 0.0f) {
        int x, y;
        SDL_GetWindowPosition(player2Window, &x, &y);
        SDL_SetWindowPosition(player2Window, x, y + static_cast<int>(m_player2Velocity * deltaTime));
    }

    // 2. Move the ball, then re-query so everything below uses where it
    //    actually ended up THIS frame, not last frame's cached position.
    int ballX, ballY, ballW, ballH;
    SDL_GetWindowPosition(ballWindow, &ballX, &ballY);
    SDL_GetWindowSize(ballWindow, &ballW, &ballH);
    ballX += static_cast<int>(m_ballVelocityX * deltaTime);
    ballY += static_cast<int>(m_ballVelocityY * deltaTime);
    SDL_SetWindowPosition(ballWindow, ballX, ballY);

    // 3. Paddle collisions, using the ball's *actual* size, not a guess
    //    derived from screen dimensions.
    auto checkPaddle = [&](SDL_Window* paddleWindow, bool isLeftPaddle) {
        int px, py, pw, ph;
        SDL_GetWindowPosition(paddleWindow, &px, &py);
        SDL_GetWindowSize(paddleWindow, &pw, &ph);

        bool overlapping = ballX < px + pw && ballX + ballW > px &&
                            ballY < py + ph && ballY + ballH > py;
        if (!overlapping) return;

        // Only bounce if actually heading toward this paddle. Without this,
        // a ball still overlapping the paddle after being pushed out (below)
        // would immediately re-trigger and flip velocity right back.
        bool movingToward = isLeftPaddle ? (m_ballVelocityX < 0.0f) : (m_ballVelocityX > 0.0f);
        if (!movingToward) return;

        // Real pong-style deflection: how far from paddle center the ball
        // hit determines the resulting angle, while total speed is preserved.
        float paddleCenter = py + ph * 0.5f;
        float ballCenter   = ballY + ballH * 0.5f;
        float relativeHit  = std::clamp((ballCenter - paddleCenter) / (ph * 0.5f), -1.0f, 1.0f);

        // Nudge the angle a little even on a dead-center, stationary-paddle hit.
        // Without this, a stationary paddle produces the exact same bounce angle
        // every time, and the ball rallies on the same horizontal line forever.
        constexpr float kRandomDeflection = 0.15f;
        float jitter = (static_cast<float>(std::rand()) / RAND_MAX * 2.0f - 1.0f) * kRandomDeflection;
        relativeHit = std::clamp(relativeHit + jitter, -1.0f, 1.0f);

        // Speed up slightly each hit, capped, so rallies can't stalemate forever.
        constexpr float kSpeedUpFactor = 1.05f;
        constexpr float kMaxSpeed = 900.0f;
        float speed = std::sqrt(m_ballVelocityX * m_ballVelocityX + m_ballVelocityY * m_ballVelocityY);
        speed = std::min(speed * kSpeedUpFactor, kMaxSpeed);

        float newVy = relativeHit * speed;
        float newVx = std::sqrt(std::max(0.0f, speed * speed - newVy * newVy));
        m_ballVelocityX = (isLeftPaddle ? 1.0f : -1.0f) * newVx;
        m_ballVelocityY = newVy;

        // Push the ball fully clear of the paddle so next tick's overlap
        // check starts false instead of immediately re-firing.
        ballX = isLeftPaddle ? (px + pw) : (px - ballW);
        SDL_SetWindowPosition(ballWindow, ballX, ballY);
    };
    checkPaddle(player1Window, /*isLeftPaddle=*/true);
    checkPaddle(player2Window, /*isLeftPaddle=*/false);

    // 4. Top/bottom walls: reverse Y and clamp position back inside bounds.
    if (ballY < usable.y) {
        m_ballVelocityY = -m_ballVelocityY;
        ballY = usable.y;
        SDL_SetWindowPosition(ballWindow, ballX, ballY);
    } else if (ballY + ballH > usable.y + usable.h) {
        m_ballVelocityY = -m_ballVelocityY;
        ballY = usable.y + usable.h - ballH;
        SDL_SetWindowPosition(ballWindow, ballX, ballY);
    }

    // 5. Safety net: if the ball ever slips past a paddle (a miss), nothing
    //    else would stop it flying off-screen forever. Re-serve from center.
    if (ballX + ballW < usable.x || ballX > usable.x + usable.w) {
        ballX = usable.x + usable.w / 2 - ballW / 2;
        ballY = usable.y + usable.h / 2 - ballH / 2;
        SDL_SetWindowPosition(ballWindow, ballX, ballY);
        m_ballVelocityX = (m_ballVelocityX < 0.0f ? 1.0f : -1.0f) * 300.0f;
        m_ballVelocityY = 0.0f;
    }
}
