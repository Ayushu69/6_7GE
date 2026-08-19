#include "../Constants.h"
#include "Game.h"
#include "../physics/Collision.h"
#include<algorithm>
#include<cmath>

Game::Game(SDL_Renderer* renderer) 
    : camera(kWindowWidth, kWindowHeight),
      player(64, 64, 4, 0.15f) {
    textures.init();

    hotbarFont = TTF_OpenFont("assets/fonts/PressStart2P.ttf", 12);
    if(hotbarFont == nullptr) {
        SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
    }

    // Load all player animation sheets
    textures.load(renderer, "anim_idle", "assets/Idle_Side-Sheet.png");
    textures.load(renderer, "anim_walk", "assets/Walk_Side-Sheet.png");
    textures.load(renderer, "anim_run",  "assets/Run_Side-Sheet.png");

    player.rect = {200, 0, 48, 64};
    player.acceleration = 3000.0f;
    player.friction = 4000.0f;
    player.maxSpeed = 200.0f;
    player.setupAnimations();

    tilemap.tileSize  = kTileSize;
    tilemap.sheetCols = kTilesetColumns;
    tilemap.loadFromCSV("assets/world.csv");
    tilemap.tileset = textures.load(renderer, "surface", "assets/surface.png");

    // Find surface at player's column and spawn above it
    int ts = tilemap.tileSize * tilemap.renderScale;
    int spawnCol = static_cast<int>(player.rect.x) / ts;
    spawnCol = std::max(0, std::min(spawnCol, tilemap.cols - 1));
    for (int row = 0; row < tilemap.rows; ++row) {
        if (tilemap.isSolid(tilemap.mapData[row][spawnCol])) {
            player.rect.y = static_cast<float>(row * ts) - player.rect.h - 4.0f;
            break;
        }
    }
}

Game::~Game() {
    textures.shutdown();
    TTF_CloseFont(hotbarFont);
}

SDL_Texture* Game::renderText(SDL_Renderer* renderer, const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(hotbarFont, text.c_str(), color);
    if (surface == nullptr) {
        SDL_Log("TTF_RenderText_Solid failed: %s", TTF_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == nullptr) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return nullptr;
    }
    return texture;
}

void Game::renderHotbar(SDL_Renderer* renderer) {
    int slotSize = 48;
    int padding = 8;
    int totalWidth = player.hotbar.size() * slotSize + (player.hotbar.size() - 1) * padding;
    int x = (kWindowWidth - totalWidth) / 2;
    int y = kWindowHeight - slotSize - padding;
    for (int i = 0; i < player.hotbar.size(); i++) {
        const InventorySlot& slot = player.hotbar[i];
        SDL_Rect rect = {x, y, slotSize, slotSize};

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &rect);

        if(slot.count > 0) {
            SDL_Texture* tex = textures.get("surface");
            if(tex) {
                int ts = tilemap.tileSize;
                int tileX = (slot.tileID % tilemap.sheetCols) * ts;
                int tileY = (slot.tileID / tilemap.sheetCols) * ts;
                SDL_Rect src = { tileX, tileY, ts, ts };
                SDL_RenderCopy(renderer, tex, &src, &rect);

                std::string countText = std::to_string(slot.count);
                SDL_Texture* countTex = renderText(renderer, countText, {255, 255, 255, 255});
                if(countTex != nullptr) {
                    int textW, textH;
                    SDL_QueryTexture(countTex, nullptr, nullptr, &textW, &textH);
                    SDL_Rect destRect = {x + slotSize - textW - 4, y + slotSize - textH - 4, textW, textH};
                    SDL_RenderCopy(renderer, countTex, nullptr, &destRect);
                    SDL_DestroyTexture(countTex);
                }

            }
        }

        if(i == player.selectedSlot) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        }else{
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        }
        SDL_RenderDrawRect(renderer, &rect);

        x += slotSize + padding;
    }
}

bool Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return false;
        if (event.type == SDL_KEYDOWN)
            if (event.key.keysym.sym == SDLK_ESCAPE) return false;

        if (event.type == SDL_MOUSEBUTTONDOWN){
            // Left click to break a tile
            if (event.button.button == SDL_BUTTON_LEFT) {
                float worldX = event.button.x + camera.x;
                float worldY = event.button.y + camera.y;

                int col = (int)(worldX) / (tilemap.tileSize * tilemap.renderScale);
                int row = (int)(worldY) / (tilemap.tileSize * tilemap.renderScale);

                int brokenID = tilemap.mapData[row][col];


                if (row >= 0 && row < tilemap.rows && col >= 0 && col < tilemap.cols) tilemap.mapData[row][col] = -1; // set to sky (empty)

                bool found = false;
                for (auto& slot : player.hotbar) {
                    if (slot.tileID == brokenID) {
                        // Add to inventory (simple stack, no max count for demo)
                        slot.count += 1;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    // Item not in hotbar, add to first empty slot
                    for (auto& slot : player.hotbar) {
                        if (slot.count == 0) {
                            slot.tileID = brokenID;
                            slot.count = 1;
                            break;
                        }
                    }
                }
            }

            // Right click to place a tile
            if (event.button.button == SDL_BUTTON_RIGHT) {
                float worldX = event.button.x + camera.x;
                float worldY = event.button.y + camera.y;
                
                int col = (int)(worldX) / (tilemap.tileSize * tilemap.renderScale);
                int row = (int)(worldY) / (tilemap.tileSize * tilemap.renderScale);

                if(row >= 0 && row < tilemap.rows && col >= 0 && col < tilemap.cols) {
                    InventorySlot& slot = player.hotbar[player.selectedSlot];
                    if (slot.count > 0) {
                        tilemap.mapData[row][col] = slot.tileID;
                        slot.count--;
                        if (slot.count == 0) slot.tileID = 0;
                    }
                }
            }
        }

        if (event.type == SDL_KEYDOWN && mode == GameMode::Gameplay) {
            if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                player.selectedSlot = event.key.keysym.sym - SDLK_1;
            } else if (event.key.keysym.sym == SDLK_0) {
                player.selectedSlot = 9; // 0 selects the 10th slot (index 9)
            }
        }
        
        if(event.type == SDL_MOUSEWHEEL && mode == GameMode::Gameplay) {
            player.selectedSlot -= event.wheel.y;
            player.selectedSlot = ((player.selectedSlot % 10) + 10) % 10;
        }
    }
    return true;
}

void Game::tick(float dt) {
    // SDL_PollEvent() in handleEvents() already pumps the event queue,
    // so SDL_GetKeyboardState() returns the up-to-date snapshot here.
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    update(keys, dt);
}

void Game::update(const Uint8* keys, float dt) {
    // Cap extreme frame times so one hitch does not create a huge physics jump.
    constexpr float kMaxFrameTime = 1.0f / 30.0f;   // ~33 ms max
    // Run physics in smaller chunks for stable collision checks.
    constexpr float kPhysicsStep = 1.0f / 120.0f;   // ~8.3 ms per sub-step

    dt = std::min(dt, kMaxFrameTime);

    const int steps = std::max(1, static_cast<int>(std::ceil(dt / kPhysicsStep)));
    const float stepDt = dt / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i) {
        player.update(keys, stepDt); //velocity + anim only now, now position change

        int ts = tilemap.tileSize * tilemap.renderScale;

        //X pass
        player.rect.x += player.velX * stepDt;

        if(player.rect.x < 0.0f) { 
            player.rect.x = 0.0f;
            player.velX = 0.0f;
        }
        if(player.rect.x + player.rect.w > kWorldTilesX * kTileSize * kRenderScale) { 
            player.rect.x = kWorldTilesX * kTileSize * kRenderScale - player.rect.w;
            player.velX = 0.0f;
        }

        player.syncCollider(); // update collider after moving
        {
            int startCol = static_cast<int>(player.colliderRect.x) / ts - 1;
            int startRow = static_cast<int>(player.colliderRect.y) / ts - 1;
            int endCol   = static_cast<int>(player.colliderRect.x + player.colliderRect.w) / ts + 1;
            int endRow   = static_cast<int>(player.colliderRect.y + player.colliderRect.h) / ts + 1;
            startCol = std::max(0, startCol);
            startRow = std::max(0, startRow);
            endCol   = std::min(tilemap.cols - 1, endCol);
            endRow   = std::min(tilemap.rows - 1, endRow);

            for (int row = startRow; row <= endRow; row++) {
                for (int col = startCol; col <= endCol; col++) {
                    int tileID = tilemap.mapData[row][col];
                    if (!tilemap.isSolid(tileID)) continue;

                    SDL_FRect tileRect = tilemap.getTileRect(col, row);

                    if (!checkCollision(player.colliderRect, tileRect)) continue;

                    float overlapLeft   = (player.colliderRect.x + player.colliderRect.w) - tileRect.x;
                    float overlapRight  = (tileRect.x + tileRect.w) - player.colliderRect.x;

                    if (overlapLeft < overlapRight) player.rect.x -= overlapLeft;
                    else player.rect.x += overlapRight;
                    player.velX = 0;

                    player.syncCollider(); // update collider after resolving collision
                }
            }
        }

        //Y pass
        player.rect.y += player.velY * stepDt;
        player.onGround = false; // reset onGround before checking collisions

        if(player.rect .y < 0.0f) {
            player.rect.y = 0.0f;
            player.velY = 0.0f;
        }
        if(player.rect.y + player.rect.h > kWorldTilesY * kTileSize * kRenderScale) {
            player.rect.y = kWorldTilesY * kTileSize * kRenderScale - player.rect.h;
            player.velY = 0.0f;
            player.onGround = true; // landed on the ground
        }
        player.syncCollider(); // update collider after moving
        {
            int startCol = static_cast<int>(player.colliderRect.x) / ts - 1;
            int startRow = static_cast<int>(player.colliderRect.y) / ts - 1;
            int endCol   = static_cast<int>(player.colliderRect.x + player.colliderRect.w) / ts + 1;
            int endRow   = static_cast<int>(player.colliderRect.y + player.colliderRect.h) / ts + 1;
            startCol = std::max(0, startCol);
            startRow = std::max(0, startRow);
            endCol   = std::min(tilemap.cols - 1, endCol);
            endRow   = std::min(tilemap.rows - 1, endRow);

            for(int row = startRow; row <= endRow; row++) {
                for(int col = startCol; col <= endCol; col++) {
                    int tileID = tilemap.mapData[row][col];
                    if(!tilemap.isSolid(tileID)) continue;

                    SDL_FRect tileRect = tilemap.getTileRect(col, row);

                    if(!checkCollision(player.colliderRect, tileRect)) continue;

                    float overlapTop= (player.colliderRect.y + player.colliderRect.h) - tileRect.y;
                    float overlapBottom  = (tileRect.y + tileRect.h) - player.colliderRect.y;

                    if (overlapTop < overlapBottom) {
                        player.rect.y -= overlapTop;
                        player.onGround = true; // landed on something
                    } else {
                        player.rect.y += overlapBottom;
                    }
                    player.velY = 0;

                    player.syncCollider(); // update collider after resolving collision
                }
            }
        }
    }
    camera.update(
        player.rect.x + player.rect.w * 0.5f,
        player.rect.y + player.rect.h * 0.5f
    );
}

void Game::render(SDL_Renderer* renderer) {
    tilemap.render(renderer, camera);

    player.render(renderer, camera, textures);

    if(kDebugDrawColliders) {
        SDL_FRect debugRect = {
            player.colliderRect.x - camera.x,
            player.colliderRect.y - camera.y,
            player.colliderRect.w,
            player.colliderRect.h
        };
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderDrawRectF(renderer, &debugRect);
    }
    
    renderHotbar(renderer);
}