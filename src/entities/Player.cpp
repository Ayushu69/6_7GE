#include "Player.h"
#include "../Constants.h"
#include "../core/Camera.h"

void Player::update(const Uint8* keys, float dt) {
    float dx = 0.0f;

    if (keys[SDL_SCANCODE_A]) dx -= 1.0f;
    if (keys[SDL_SCANCODE_D]) dx += 1.0f;

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

    // jump
    if(keys[SDL_SCANCODE_SPACE] && onGround) {
        velY = -jumpForce;
        onGround = false;
    }

    // move
    rect.x += velX * dt;
    rect.y += velY * dt;

    // world bounds
    if (rect.x < 0.0f) { rect.x = 0.0f; velX = 0.0f; }
    if (rect.x + rect.w > kWorldTilesX * kTileSize * kRenderScale) { rect.x = kWorldTilesX * kTileSize * kRenderScale - rect.w; velX = 0.0f; }
    if (rect.y < 0.0f) { rect.y = 0.0f; velY = 0.0f; }
    if (rect.y + rect.h > kWorldTilesY * kTileSize * kRenderScale) { rect.y = kWorldTilesY * kTileSize * kRenderScale - rect.h; velY = 0.0f; }

    // sync collider
    colliderRect = {
        rect.x + 5.0f,
        rect.y + 5.0f,
        rect.w - 10.0f,
        rect.h - 10.0f
    };

    // reset onGround, will be set again during collision resolution if still colliding with ground
    onGround = false;
}

//     //Normalize input
//     const float lenSq = dx*dx + dy*dy;
//     if(lenSq > 1.0f){
//         const float len = SDL_sqrtf(lenSq);
//         dx /= len;
//         dy /= len;
//     }

//     // acceleration
//     if(dx != 0) velX += dx*acceleration*dt;
//     if(dy != 0) velY += dy*acceleration*dt;

//     // friction
//     if(dx == 0){
//         if(velX > 0){
//             velX -= friction * dt;
//             if(velX < 0) velX = 0;
//         }
//         else if(velX < 0){
//             velX += friction * dt;
//             if(velX > 0) velX = 0;
//         }
//     }

//     if(dy == 0){
//         if(velY > 0){
//             velY -= friction * dt;
//             if(velY < 0) velY = 0;
//         }
//         else if(velY < 0){
//             velY += friction * dt;
//             if(velY > 0) velY = 0;
//         }
//     }

//     // clamp
//     if(velX > maxSpeed) velX = maxSpeed;
//     if(velX < -maxSpeed) velX = -maxSpeed;

//     if(velY > maxSpeed) velY = maxSpeed;
//     if(velY < -maxSpeed) velY = -maxSpeed;

//     // move
//     rect.x += velX * dt;
//     rect.y += velY * dt;

//     // bounds
//     if (rect.x < 0.0f) { rect.x = 0.0f; velX = 0.0f; }
//     if (rect.y < 0.0f) { rect.y = 0.0f; velY = 0.0f; }
//     if (rect.x + rect.w > kWorldTilesX * kTileSize * kRenderScale) { rect.x = kWorldTilesX * kTileSize * kRenderScale - rect.w; velX = 0.0f; }
//     if (rect.y + rect.h > kWorldTilesY * kTileSize * kRenderScale) { rect.y = kWorldTilesY * kTileSize * kRenderScale - rect.h; velY = 0.0f; }

//     // after moving rect, sync collider
//     float insetX = 5.0f, insetY = 5.0f;
//     colliderRect = {
//         rect.x + insetX,
//         rect.y + insetY,
//         rect.w - insetX * 2,
//         rect.h - insetY * 2
//     };
// }

void Player::render(SDL_Renderer* renderer, const Camera& camera, SDL_Texture* tex) const {
    SDL_FRect screenRect = {
        rect.x - camera.x,
        rect.y - camera.y,
        rect.w,
        rect.h
    };

    if (tex) {
        SDL_RenderCopyF(renderer, tex, nullptr, &screenRect);
        // nullptr for src means "use the whole texture"
    } else {
        // fallback to colored rect if no texture
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRectF(renderer, &screenRect);
    }
}