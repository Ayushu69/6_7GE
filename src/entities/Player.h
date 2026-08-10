#pragma once
#include <SDL2/SDL.h>
#include <array>
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

struct InventorySlot {
    int tileID = 0;
    int count = 0;
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

    static constexpr float kColliderWidth = 18.0f;
    static constexpr float kColliderHeight = 40.0f;

    AnimState animState = AnimState::Idle;
    Animator animator;

    Player(int frameWidth, int frameHeight, int totalFrames, float frameDelay, int row = 0);

    void setupAnimations();
    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer, const Camera& camera, const TextureManager& textures) const;

    // Get the texture key for the current animation state
    std::string getAnimTexture() const;

    std::array<InventorySlot, 10> hotbar; // 10 slots for building blocks
    int selectedSlot = 0; // Index of the currently selected hotbar slot
};
