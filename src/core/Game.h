#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "../Constants.h"
#include "Camera.h"
#include "../entities/Box.h"
#include "../entities/Player.h"
#include "TextureManager.h"

class Game {
public:
    TextureManager textures;
    Player player;
    std::vector<Box> boxes;
    Camera camera;

    Game(SDL_Renderer* renderer);

    bool handleEvents();
    void tick(float dt);

    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer);
};