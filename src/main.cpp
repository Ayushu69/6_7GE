#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <bits/stdc++.h>
#include "core/Game.h"

using namespace std;

// EVENTS
void handleEvents(bool& running){
    SDL_Event event;

    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) running = false;
        
        if (event.type == SDL_KEYDOWN){
            if(event.key.keysym.sym == SDLK_ESCAPE) running = false;
        }
    }
}

// GAME UPDATE
void tick(Game& game, float deltaTime){
    SDL_PumpEvents();
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    game.update(keys, deltaTime);
}

// GLOBAL RENDERER
void render(SDL_Renderer* renderer, Game& game) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    game.render(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "six_sevenGE",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, 
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    Game game;
    bool running = true;

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double dt = 0;

    while (running) {
        last = now;
        now = SDL_GetPerformanceCounter();
        dt = (double)(now - last) / SDL_GetPerformanceFrequency();

        // GAME SYSTEM (EVENTS, UPDATION, RENDERING)
        handleEvents(running);
        tick(game, dt);
        render(renderer, game);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}