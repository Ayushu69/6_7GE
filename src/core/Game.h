#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
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
    ~Game();

    bool handleEvents();
    void tick(float dt);

    void update(const Uint8* keys, float dt);
    void render(SDL_Renderer* renderer);

    void renderHotbar(SDL_Renderer* renderer); // Render the player's hotbar at the bottom of the screen

    enum class GameMode { Gameplay, Menu }; 
    GameMode mode = GameMode::Gameplay; 

private:
    TTF_Font* hotbarFont = nullptr;
    SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, SDL_Color color); // Helper function to render text to a texture
    struct MiningProgress {
        int row = -1, col = -1;
        int hitsSoFar = 0;
        float timeSinceLastHit = 0.0f;
    };
    MiningProgress miningProgress; // Track the current mining progress
};