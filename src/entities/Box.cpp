#include "Box.h"

void Box::render(SDL_Renderer* renderer, const Camera& camera, SDL_Texture* tex) const {
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