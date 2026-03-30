#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "core/Game.h"

// GLOBAL RENDERER
void render(SDL_Renderer* renderer, Game& game) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    game.render(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {

    // 1) SDL init check
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "six_sevenGE",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    // 2) Window creation check
    if (window == nullptr) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, 
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // 3) Renderer creation check
    if (renderer == nullptr) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Game game;
    bool running = true;

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double dt = 0.0f;
    
    while (running) {
        last = now;
        now = SDL_GetPerformanceCounter();
        dt = (double)(now - last) / SDL_GetPerformanceFrequency();

        // GAME SYSTEM (EVENTS, UPDATION, RENDERING)
        running = game.handleEvents();
        game.tick(window, static_cast<float>(dt));
        render(renderer, game);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}