#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "../Constants.h"
#include "Camera.h"
#include "../entities/Player.h"
#include "TextureManager.h"
#include "Tilemap.h"

class Game {
public:
    TextureManager textures;
    Player player;
    Tilemap tilemap;
    Camera camera;

    Game(SDL_Renderer* renderer);

    bool handleEvents();
    void tick(float dt);

    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer);
    void renderHotbar(SDL_Renderer* renderer);

    enum class GameMode { Gameplay, Menu };
    GameMode mode = GameMode::Gameplay;
};