#include "Game.h"
#include "../physics/Collision.h"
#include<algorithm>
#include<cmath>

void Box::render(SDL_Renderer* renderer) const{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRectF(renderer, &rect);
}

Game::Game(){
    player.rect = {100, 100, 50, 50};
    player.acceleration = 2000.0f;
    player.friction = 1200.0f;
    player.maxSpeed = 400.0f;

    boxes.push_back({{300, 200, 100, 100}});
    boxes.push_back({{500, 100, 150, 50}});
    boxes.push_back({{200, 400, 200, 50}});
}

void Game::update(const Uint8* keys, float dt, float worldWidth, float worldHeight) {
    // Cap extreme frame times so one hitch does not create a huge physics jump.
    constexpr float kMaxFrameTime = 1.0f / 30.0f;   // ~33 ms max
    // Run physics in smaller chunks for stable collision checks.
    constexpr float kPhysicsStep = 1.0f / 120.0f;   // ~8.3 ms per sub-step

    dt = std::min(dt, kMaxFrameTime);

    const int steps = std::max(1, static_cast<int>(std::ceil(dt / kPhysicsStep)));
    const float stepDt = dt / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i) {
        player.update(keys, stepDt, worldWidth, worldHeight);

        //collision resolution
        for (const auto& box : boxes) {
            if (checkCollision(player.rect, box.rect)) {

                float overlapLeft   = (player.rect.x + player.rect.w) - box.rect.x;
                float overlapRight  = (box.rect.x + box.rect.w) - player.rect.x;
                float overlapTop    = (player.rect.y + player.rect.h) - box.rect.y;
                float overlapBottom = (box.rect.y + box.rect.h) - player.rect.y;

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
}

void Game::render(SDL_Renderer* renderer) {
    for(const auto& box: boxes) box.render(renderer);
    player.render(renderer);
}