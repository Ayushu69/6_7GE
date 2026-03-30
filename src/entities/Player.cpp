#include "Player.h"

void Player::update(const Uint8* keys, float dt, float worldWidth, float worldHeight) {
    float dx = 0.0f, dy = 0.0f;

    if(keys[SDL_SCANCODE_W]) dy -= 1.0f;
    if(keys[SDL_SCANCODE_S]) dy += 1.0f;
    if(keys[SDL_SCANCODE_A]) dx -= 1.0f;
    if(keys[SDL_SCANCODE_D]) dx += 1.0f;

    //Normalize input
    const float lenSq = dx*dx + dy*dy;
    if(lenSq > 1.0f){
        const float len = SDL_sqrtf(lenSq);
        dx /= len;
        dy /= len;
    }

    // acceleration
    if(dx != 0) velX += dx*acceleration*dt;
    if(dy != 0) velY += dy*acceleration*dt;

    // friction
    if(dx == 0){
        if(velX > 0){
            velX -= friction * dt;
            if(velX < 0) velX = 0;
        }
        else if(velX < 0){
            velX += friction * dt;
            if(velX > 0) velX = 0;
        }
    }

    if(dy == 0){
        if(velY > 0){
            velY -= friction * dt;
            if(velY < 0) velY = 0;
        }
        else if(velY < 0){
            velY += friction * dt;
            if(velY > 0) velY = 0;
        }
    }

    // clamp
    if(velX > maxSpeed) velX = maxSpeed;
    if(velX < -maxSpeed) velX = -maxSpeed;

    if(velY > maxSpeed) velY = maxSpeed;
    if(velY < -maxSpeed) velY = -maxSpeed;

    // move
    rect.x += velX * dt;
    rect.y += velY * dt;

    // bounds
    if (rect.x < 0.0f) { rect.x = 0.0f; velX = 0.0f; }
    if (rect.y < 0.0f) { rect.y = 0.0f; velY = 0.0f; }
    if (rect.x + rect.w > worldWidth) { rect.x = worldWidth - rect.w; velX = 0.0f; }
    if (rect.y + rect.h > worldHeight) { rect.y = worldHeight - rect.h; velY = 0.0f; }
}

void Player::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRectF(renderer, &rect);
}