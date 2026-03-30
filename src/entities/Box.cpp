#include "Box.h"

void Box::render(SDL_Renderer* renderer, const Camera& camera) const {
    SDL_FRect screenRect = {
        rect.x - camera.x,
        rect.y - camera.y,
        rect.w,
        rect.h
    };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRectF(renderer, &screenRect);
}