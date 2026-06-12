#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "../core/Animator.h"

struct Camera;
class TextureManager;

// Animation states the player can be in
enum class AnimState {
    Idle,
    Walk,
    Run,
    Jump
};

struct Player {
    SDL_FRect rect;
    SDL_FRect colliderRect;

    float velX = 0.0f, velY = 0.0f;
    float acceleration = 0.0f, friction = 0.0f, maxSpeed = 0.0f, sprintSpeed = 300.0f;
    float gravityAccel = 1800.0f;
    float jumpForce = 700.0f;
    bool onGround = false;
    bool wasSpacePressed = false;
    bool facingRight = true;
    bool isSprinting = false;

    AnimState animState = AnimState::Idle;
    Animator animator;

    Player(int frameWidth, int frameHeight, int totalFrames, float frameDelay, int row = 0);

    void setupAnimations();
    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer, const Camera& camera, const TextureManager& textures) const;

    // Get the texture key for the current animation state
    std::string getAnimTexture() const;
};