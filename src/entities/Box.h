#pragma once
#include <SDL2/SDL.h>

struct Box {
    SDL_FRect rect;
    void render(SDL_Renderer* renderer) const;
};