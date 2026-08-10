#include "Player.h"
#include "../Constants.h"
#include "../core/Camera.h"
#include "../core/TextureManager.h"

Player::Player(int frameWidth, int frameHeight, int totalFrames, float frameDelay, int row)
    : animator(frameWidth, frameHeight, totalFrames, frameDelay, row) {}

void Player::setupAnimations() {
    // Register all animation clips
    // All sheets are single-row, 64x64 per frame
    animator.addClip("idle", {64, 64, 4, 0.15f, true});   // 4 frames, slow
    animator.addClip("walk", {64, 64, 6, 0.12f, true});   // 6 frames
    animator.addClip("run",  {64, 64, 6, 0.08f, true});   // 6 frames, faster
    animator.addClip("jump", {64, 64, 6, 0.10f, false});  // use run sheet, hold last frame

    // Start with idle
    animator.play("idle");
}

std::string Player::getAnimTexture() const {
    switch (animState) {
        case AnimState::Idle: return "anim_idle";
        case AnimState::Walk: return "anim_walk";
        case AnimState::Run:  return "anim_run";
        case AnimState::Jump: return "anim_run";  // reuse run sheet for jump
        default:              return "anim_idle";
    }
}

void Player::update(const Uint8* keys, float dt) {
    float dx = 0.0f;

    if (keys[SDL_SCANCODE_A]) dx -= 1.0f;
    if (keys[SDL_SCANCODE_D]) dx += 1.0f;

    // update facing direction
    if (dx > 0.0f) facingRight = true;
    else if (dx < 0.0f) facingRight = false;

    // sprint state
    isSprinting = keys[SDL_SCANCODE_LSHIFT] && dx != 0.0f;

    // horizontal acceleration
    if(dx != 0.0f) velX += dx * acceleration * dt;

    // horizontal friction
    if(dx == 0.0f) {
        if(velX > 0) {
            velX -= friction * dt;
            if(velX < 0) velX = 0;
        }else if(velX < 0) {
            velX += friction * dt;
            if(velX > 0) velX = 0;
        }
    }

    // clamp horizontal speed
    float currentMax = keys[SDL_SCANCODE_LSHIFT] ? sprintSpeed : maxSpeed;
    if(velX > currentMax) velX = currentMax;
    if(velX < -currentMax) velX = -currentMax;

    //gravity
    velY += gravityAccel * dt;

    // jump (edge-triggered: only on fresh press, not hold)
    bool spaceNow = keys[SDL_SCANCODE_SPACE];
    if(spaceNow && !wasSpacePressed && onGround) {
        velY = -jumpForce;
        onGround = false;
    }
    wasSpacePressed = spaceNow;

    // ── Determine animation state ────────────────────────────
    AnimState newState;
    if (!onGround) {
        newState = AnimState::Jump;
    } else if (dx != 0.0f && isSprinting) {
        newState = AnimState::Run;
    } else if (dx != 0.0f) {
        newState = AnimState::Walk;
    } else {
        newState = AnimState::Idle;
    }

    // Switch clip if state changed
    if (newState != animState) {
        animState = newState;
        switch (animState) {
            case AnimState::Idle: animator.play("idle"); break;
            case AnimState::Walk: animator.play("walk"); break;
            case AnimState::Run:  animator.play("run");  break;
            case AnimState::Jump: animator.play("jump"); break;
        }
    }

    animator.update(dt);
}

void Player::syncCollider() {
    float feetY = rect.y + rect.h - kFeetPaddingBelowSprite;
    colliderRect = {
        rect.x + (rect.w - kPlayerColliderWidth) * 0.5f,
        feetY - kPlayerColliderHeight,
        kPlayerColliderWidth,
        kPlayerColliderHeight
    };
}

void Player::render(SDL_Renderer* renderer, const Camera& camera, const TextureManager& textures) const {
    SDL_FRect screenRect = {
        rect.x - camera.x,
        rect.y - camera.y,
        rect.w,
        rect.h
    };

    // Get the right texture for the current animation state
    SDL_Texture* tex = textures.get(getAnimTexture());

    if (tex) {
        SDL_Rect src = animator.getSrcRect();
        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyExF(renderer, tex, &src, &screenRect, 0.0, nullptr, flip);
    } else {
        // fallback to colored rect if no texture
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRectF(renderer, &screenRect);
    }
}