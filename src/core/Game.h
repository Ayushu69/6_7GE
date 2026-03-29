#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "../entities/Player.h"

struct Box {
    SDL_FRect rect;
    void render(SDL_Renderer* renderer) const;
};

class Game {
public:
    Player player;
    std::vector<Box> boxes;

    Game();

    void update(const Uint8* keys, float dt, float worldWidth, float worldHeight);
    void render(SDL_Renderer* renderer);
};