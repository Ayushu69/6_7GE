#pragma once
#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include<unordered_map>
#include "Camera.h"
struct TileProperties {
    int hardness = 1;
    int dropID = -1;
};
struct Tilemap{
    Tilemap();
    int tileSize = 16;
    int renderScale = kRenderScale;
    int cols = 0;
    int rows = 0;

    std::unordered_map<int, TileProperties> tileProperties; // Map of tileID to its properties
    TileProperties getTileProperties(int tileID) const;

    std::vector<std::vector<int>> mapData;

    SDL_Texture* tileset = nullptr;
    int sheetCols = 0; // how many tiles wide spritesheet is

    bool loadFromCSV(const std::string& path);
    void render(SDL_Renderer* renderer, const Camera& camera) const;
    bool isSolid(int tileID) const;
    // returns the world-space SDL_FRect of a tile at (col, row)
    SDL_FRect getTileRect(int col, int row) const {
        return {
            static_cast<float>(col * tileSize * renderScale),
            static_cast<float>(row * tileSize * renderScale),
            static_cast<float>(tileSize * renderScale),
            static_cast<float>(tileSize * renderScale)
        };
    }

    SDL_Rect getSrcRectForTile(int tileID) const {
        int srcX = (tileID % sheetCols) * (tileSize + kTileMargin);
        int srcY = (tileID / sheetCols) * (tileSize + kTileMargin);
        return { srcX, srcY, tileSize, tileSize };
    };
};