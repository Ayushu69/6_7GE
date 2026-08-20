#include "Tilemap.h"
#include "../Constants.h"
#include<fstream>
#include<sstream>
#include<SDL2/SDL.h>

Tilemap::Tilemap() {
    tileProperties[0] = { 2, 1 };  // grass: 2 hits, drops dirt
    tileProperties[1] = { 2, 1 };  // dirt: 2 hits, drops dirt
    tileProperties[2] = { 4, 2 };  // stone: 4 hits, drops stone
    tileProperties[3] = { 3, 3 };  // wood planks: 3 hits, drops itself
    tileProperties[4] = { 6, 4 };  // brick: 6 hits, drops itself
    tileProperties[5] = { 6, 5 };  // cobblestone: 6 hits, drops itself
}

bool Tilemap::loadFromCSV(const std::string& path) {
    std::ifstream file(path);
    if(!file.is_open()){
        SDL_Log("Tilemap: Failed to open %s", path.c_str());
        return false;
    }

    mapData.clear();

    std::string line;
    while(std::getline(file, line)){
        if(line.empty()) continue;

        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;

        while(std::getline(ss, cell, ',')){
            row.push_back(std::stoi(cell));
        }
        mapData.push_back(row);
    }

    rows = static_cast<int>(mapData.size());
    cols = rows > 0 ? static_cast<int>(mapData[0].size()) : 0;

    SDL_Log("tilemap Loaded: %d cols x %d rows", cols, rows);
    return true;
}

void Tilemap::render(SDL_Renderer* renderer, const Camera& camera) const {
    // camera culling - only render visible tiles
    int startCol = static_cast<int>(camera.x / (tileSize * renderScale));
    int startRow = static_cast<int>(camera.y / (tileSize * renderScale));
    int endCol   = static_cast<int>((camera.x + camera.w) / (tileSize * renderScale)) + 1;
    int endRow   = static_cast<int>((camera.y + camera.h) / (tileSize * renderScale)) + 1;

    // clamp to map bounds
    startCol = std::max(0, startCol);
    startRow = std::max(0, startRow);
    endCol   = std::min(cols - 1, endCol);
    endRow   = std::min(rows - 1, endRow);

    for (int row = startRow; row <= endRow; row++) {
        for (int col = startCol; col <= endCol; col++) {
            int tileID = mapData[row][col];

            // skip only invalid/negative tiles
            if (tileID < 0) continue;

            // world position of this tile
            float worldX = static_cast<float>(col * tileSize * renderScale);
            float worldY = static_cast<float>(row * tileSize * renderScale);

            // screen position
            SDL_FRect dst = {
                worldX - camera.x,
                worldY - camera.y,
                static_cast<float>(tileSize * renderScale),
                static_cast<float>(tileSize * renderScale)
            };

            if (tileset && sheetCols > 0) {
                // crop the right tile from spritesheet
                int srcX = (tileID % sheetCols) * (tileSize + kTileMargin);
                int srcY = (tileID / sheetCols) * (tileSize + kTileMargin);
                SDL_Rect src = { srcX, srcY, tileSize, tileSize };
                SDL_RenderCopyF(renderer, tileset, &src, &dst);
            } else {
                // fallback: colored rect
                SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
                SDL_RenderFillRectF(renderer, &dst);
            }
        }
    }
}

bool Tilemap::isSolid(int tileID) const {
    // -1 = sky (not solid), 0+ = solid blocks (grass, dirt, rock)
    return tileID >= 0;
}

TileProperties Tilemap::getTileProperties(int tileID) const {
    auto it = tileProperties.find(tileID);
    if(it != tileProperties.end()){
        return it->second;
    }
    return TileProperties{1, tileID};
}