#pragma once
#include <SDL2/SDL.h>

struct DroppedItem {
    SDL_FRect rect;
    float velY = 0.0f;
    int tileID;
    bool onGround = false;
};