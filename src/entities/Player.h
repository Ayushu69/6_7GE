#pragma once
#include <SDL2/SDL.h>

struct Camera;

struct Player {
    SDL_FRect rect;

    float velX = 0.0f, velY = 0.0f;
    float acceleration = 0.0f, friction = 0.0f, maxSpeed = 0.0f;

    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer, const Camera& camera) const;
};