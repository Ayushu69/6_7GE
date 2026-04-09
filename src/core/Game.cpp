#include "../Constants.h"
#include "Game.h"
#include "../physics/Collision.h"
#include<algorithm>
#include<cmath>

Game::Game(SDL_Renderer* renderer) : camera(kWindowWidth, kWindowHeight) {
    textures.init();
    textures.load(renderer, "player", "assets/player.png");
    textures.load(renderer, "box",    "assets/box.png");
    player.rect = {100, 100, 50, 50};
    player.acceleration = 2000.0f;
    player.friction = 1200.0f;
    player.maxSpeed = 400.0f;

    boxes.push_back({{300, 200, 100, 100}});
    boxes.push_back({{500, 100, 150, 50}});
    boxes.push_back({{200, 400, 200, 50}});
}

bool Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return false;
        if (event.type == SDL_KEYDOWN)
            if (event.key.keysym.sym == SDLK_ESCAPE) return false;
    }
    return true;
}

void Game::tick(float dt) {
    SDL_PumpEvents();
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    update(keys, dt);
}

void Game::update(const Uint8* keys, float dt) {
    // Cap extreme frame times so one hitch does not create a huge physics jump.
    constexpr float kMaxFrameTime = 1.0f / 30.0f;   // ~33 ms max
    // Run physics in smaller chunks for stable collision checks.
    constexpr float kPhysicsStep = 1.0f / 120.0f;   // ~8.3 ms per sub-step

    dt = std::min(dt, kMaxFrameTime);

    const int steps = std::max(1, static_cast<int>(std::ceil(dt / kPhysicsStep)));
    const float stepDt = dt / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i) {
        player.update(keys, stepDt);

        //collision resolution
        for (const auto& box : boxes) {
            if (checkCollision(player.colliderRect, box.rect)) {

                float overlapLeft   = (player.colliderRect.x + player.colliderRect.w) - box.rect.x;
                float overlapRight  = (box.rect.x + box.rect.w) - player.colliderRect.x;
                float overlapTop    = (player.colliderRect.y + player.colliderRect.h) - box.rect.y;
                float overlapBottom = (box.rect.y + box.rect.h) - player.colliderRect.y;

                float minX = std::min(overlapLeft, overlapRight);
                float minY = std::min(overlapTop, overlapBottom);

                if (minX < minY) {
                    if (overlapLeft < overlapRight)
                        player.rect.x -= overlapLeft;
                    else
                        player.rect.x += overlapRight;

                    player.velX = 0;
                } else {
                    if (overlapTop < overlapBottom)
                        player.rect.y -= overlapTop;
                    else
                        player.rect.y += overlapBottom;

                    player.velY = 0;
                }
            }
        }
    }
    camera.update(
        player.rect.x + player.rect.w*0.5f,
        player.rect.y + player.rect.h*0.5f
    );
}

void Game::render(SDL_Renderer* renderer) {
    for(const auto& box: boxes)
        box.render(renderer, camera, textures.get("box"));

    player.render(renderer, camera, textures.get("player"));
}