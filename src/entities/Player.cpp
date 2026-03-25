#include "Player.h"

void Player::update(const Uint8* keys, float dt){
    dx = dy = 0.0f;

    if(keys[SDL_SCANCODE_W]) dy -= 1;
    if(keys[SDL_SCANCODE_S]) dy += 1;
    if(keys[SDL_SCANCODE_A]) dx -= 1;
    if(keys[SDL_SCANCODE_D]) dx += 1;

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
    if (rect.x < 0) { rect.x = 0; velX = 0; }
    if (rect.y < 0) { rect.y = 0; velY = 0; }
    if (rect.x + rect.w > 800) { rect.x = 800 - rect.w; velX = 0; }
    if (rect.y + rect.h > 600) { rect.y = 600 - rect.h; velY = 0; }
}

void Player::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRectF(renderer, &rect);
}