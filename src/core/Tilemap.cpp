#include "Tilemap.h"
#include<fstream>
#include<sstream>
#include<SDL2/SDL.h>

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
    int startCol = static_cast<int>(camera.x / tileSize);
    int startRow = static_cast<int>(camera.y / tileSize);
    int endCol   = static_cast<int>((camera.x + camera.w) / tileSize) + 1;
    int endRow   = static_cast<int>((camera.y + camera.h) / tileSize) + 1;

    // clamp to map bounds
    startCol = std::max(0, startCol);
    startRow = std::max(0, startRow);
    endCol   = std::min(cols - 1, endCol);
    endRow   = std::min(rows - 1, endRow);

    for (int row = startRow; row <= endRow; row++) {
        for (int col = startCol; col <= endCol; col++) {
            int tileID = mapData[row][col];

            // 0 = air, skip
            if (tileID == 0) continue;

            // world position of this tile
            float worldX = static_cast<float>(col * tileSize);
            float worldY = static_cast<float>(row * tileSize);

            // screen position
            SDL_FRect dst = {
                worldX - camera.x,
                worldY - camera.y,
                static_cast<float>(tileSize),
                static_cast<float>(tileSize)
            };

            if (tileset && sheetCols > 0) {
                // crop the right tile from spritesheet
                int srcX = (tileID % sheetCols) * tileSize;
                int srcY = (tileID / sheetCols) * tileSize;
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
    // 0 = air (not solid)
    // everything else is solid for now
    return tileID != 0;
}