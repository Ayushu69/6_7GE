#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "../Constants.h"
#include "Camera.h"
#include "../entities/Box.h"
#include "../entities/Player.h"

class Game {
public:
    Player player;
    std::vector<Box> boxes;
    Camera camera;

    Game(float screenW, float screenH);

    bool handleEvents();
    void tick(SDL_Window* window, float dt);

    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer);
};