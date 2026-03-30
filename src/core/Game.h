#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "../entities/Box.h"
#include "../entities/Player.h"

class Game {
public:
    Player player;
    std::vector<Box> boxes;

    Game();

    bool handleEvents();
    void tick(SDL_Window* window, float dt);

    void update(const Uint8* keys, float dt, float worldWidth, float worldHeight);
    void render(SDL_Renderer* renderer);
};