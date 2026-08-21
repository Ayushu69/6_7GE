#pragma once

constexpr int   kWindowWidth  = 1280;
constexpr int   kWindowHeight = 720;

// Tileset
constexpr int kTileSize       = 16;
constexpr int kTileMargin     = 0;
constexpr int kTilesetColumns = 4;
constexpr int kTilesetRows    = 1;
constexpr int kRenderScale    = 3;

// World in tiles
constexpr int kWorldTilesX = 200;
constexpr int kWorldTilesY = 100;

constexpr bool kDebugDrawColliders = true; // set to false to disable debug collider rendering

constexpr int kMaxReachPixels = 80; // maximum distance in tiles the player can reach to break/place blocks

constexpr int kDroppedItemSize = 24; // size of dropped item in pixels (width and height)

constexpr float kGravityAccel = 1800.0f;