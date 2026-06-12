#include "../Constants.h"
#include "Game.h"
#include "../physics/Collision.h"
#include<algorithm>
#include<cmath>

Game::Game(SDL_Renderer* renderer) 
    : camera(kWindowWidth, kWindowHeight),
      player(64, 64, 4, 0.15f) {
    textures.init();

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
    tilemap.tileset = textures.load(renderer, "tileset", "assets/tileset.png");

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

bool Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return false;
        if (event.type == SDL_KEYDOWN)
            if (event.key.keysym.sym == SDLK_ESCAPE) return false;
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
        player.update(keys, stepDt);

        // Reset each sub-step; collision resolution below will re-set if still on ground.
        player.onGround = false;

        // --- tilemap collision ---
        int ts = tilemap.tileSize * tilemap.renderScale;

        // get tile range around player collider
        int startCol = static_cast<int>(player.colliderRect.x) / ts - 1;
        int startRow = static_cast<int>(player.colliderRect.y) / ts - 1;
        int endCol   = static_cast<int>(player.colliderRect.x + player.colliderRect.w) / ts + 1;
        int endRow   = static_cast<int>(player.colliderRect.y + player.colliderRect.h) / ts + 1;

        // clamp to map bounds
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
                float overlapTop    = (player.colliderRect.y + player.colliderRect.h) - tileRect.y;
                float overlapBottom = (tileRect.y + tileRect.h) - player.colliderRect.y;

                float minX = std::min(overlapLeft, overlapRight);
                float minY = std::min(overlapTop, overlapBottom);

                if (minX < minY) {
                    if (overlapLeft < overlapRight) player.rect.x -= overlapLeft;
                    else player.rect.x += overlapRight;
                    player.velX = 0;
                } else {
                    if (overlapTop < overlapBottom){
                         player.rect.y -= overlapTop;
                         player.onGround = true; // landed on something
                    }else {
                        player.rect.y += overlapBottom;
                    }
                    player.velY = 0;
                }

                // re-sync collider after push
                player.colliderRect = {
                    player.rect.x + 5.0f,
                    player.rect.y + 5.0f,
                    player.rect.w - 10.0f,
                    player.rect.h - 10.0f
                };
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
}