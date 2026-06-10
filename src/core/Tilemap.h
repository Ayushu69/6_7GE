#pragma once
#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include "Camera.h"

struct Tilemap{
    int tileSize = 64;
    int cols = 0;
    int rows = 0;

    std::vector<std::vector<int>> mapData;

    SDL_Texture* tileset = nullptr;
    int sheetCols = 0; // how many tiles wide spritesheet is

    bool loadFromCSV(const std::string& path);
    void render(SDL_Renderer* renderer, const Camera& camera) const;
    bool isSolid(int tileID) const;
    // returns the world-space SDL_FRect of a tile at (col, row)
    SDL_FRect getTileRect(int col, int row) const {
        return {
            static_cast<float>(col * tileSize),
            static_cast<float>(row * tileSize),
            static_cast<float>(tileSize),
            static_cast<float>(tileSize)
        };
    }
};