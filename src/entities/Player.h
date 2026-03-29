#pragma once
#include <SDL2/SDL.h>

struct Player {
    SDL_FRect rect;

    float velX = 0.0f, velY = 0.0f;
    float acceleration, friction, maxSpeed;

    float dx, dy;

    void update(const Uint8* keys, float dt, float worldWidth, float worldHeight);
    void render(SDL_Renderer* renderer) const;
};