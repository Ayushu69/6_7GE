#pragma once
#include <SDL2/SDL.h>
#include "../core/Camera.h"

struct Box {
    SDL_FRect rect;
    void render(SDL_Renderer* renderer, const Camera& camera, SDL_Texture* tex) const;
};