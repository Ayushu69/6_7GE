#include "../Constants.h"
#include "Game.h"
#include "../physics/Collision.h"
#include<algorithm>
#include<cmath>

Game::Game(SDL_Renderer* renderer) : camera(kWindowWidth, kWindowHeight) {
    textures.init();
    textures.load(renderer, "player", "assets/player.png");
    textures.load(renderer, "box",    "assets/box.png");
    player.rect = {200, 200, 50, 50};
    player.acceleration = 2000.0f;
    player.friction = 1200.0f;
    player.maxSpeed = 400.0f;
    tilemap.tileSize  = 64;
    tilemap.sheetCols = 8; // tileset.png is 512px wide / 64 = 8 cols
    tilemap.loadFromCSV("assets/arena.csv");
    tilemap.tileset = textures.load(renderer, "tileset", "assets/tileset.png");

    boxes.push_back({{300, 200, 100, 100}});
    boxes.push_back({{500, 100, 150, 50}});
    boxes.push_back({{200, 400, 200, 50}});
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
    SDL_PumpEvents();
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

        //collision resolution
        for (const auto& box : boxes) {
            if (checkCollision(player.colliderRect, box.rect)) {

                float overlapLeft   = (player.colliderRect.x + player.colliderRect.w) - box.rect.x;
                float overlapRight  = (box.rect.x + box.rect.w) - player.colliderRect.x;
                float overlapTop    = (player.colliderRect.y + player.colliderRect.h) - box.rect.y;
                float overlapBottom = (box.rect.y + box.rect.h) - player.colliderRect.y;

                float minX = std::min(overlapLeft, overlapRight);
                float minY = std::min(overlapTop, overlapBottom);

                if (minX < minY) {
                    if (overlapLeft < overlapRight)
                        player.rect.x -= overlapLeft;
                    else
                        player.rect.x += overlapRight;

                    player.velX = 0;
                } else {
                    if (overlapTop < overlapBottom)
                        player.rect.y -= overlapTop;
                    else
                        player.rect.y += overlapBottom;

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
        // --- tilemap collision ---
        int ts = tilemap.tileSize;

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
                    else                            player.rect.x += overlapRight;
                    player.velX = 0;
                } else {
                    if (overlapTop < overlapBottom) player.rect.y -= overlapTop;
                    else                            player.rect.y += overlapBottom;
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
    tilemap.render(renderer, camera);  // ← add this first

    for(const auto& box: boxes)
        box.render(renderer, camera, textures.get("box"));

    player.render(renderer, camera, textures.get("player"));
}