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

static constexpr float kColliderWidth = 18.0f;
static constexpr float kColliderHeight = 40.0f;

constexpr bool kDebugDrawColliders = true; // set to false to disable debug collider rendering