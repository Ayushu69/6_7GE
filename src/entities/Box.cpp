#include "Box.h"

void Box::render(SDL_Renderer* renderer) const{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRectF(renderer, &rect);
}