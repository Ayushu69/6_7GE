#pragma once
#include <SDL2/SDL.h>

struct Camera;

struct Player {
    SDL_FRect rect;
    SDL_FRect colliderRect;

    float velX = 0.0f, velY = 0.0f;
    float acceleration = 0.0f, friction = 0.0f, maxSpeed = 0.0f, sprintSpeed = 300.0f;
    float gravityAccel = 1800.0f;
    float jumpForce = 700.0f;
    bool onGround = false;

    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer, const Camera& camera, SDL_Texture* tex) const;
};