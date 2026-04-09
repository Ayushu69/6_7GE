#include "TextureManager.h"
#include <SDL2/SDL.h>

bool TextureManager::init() {
    // IMG_INIT_PNG tells SDL_image to support PNG files
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("IMG_Init failed: %s", IMG_GetError());
        return false;
    }
    return true;
}

void TextureManager::shutdown() {
    for (auto& [id, tex] : mTextures)
        SDL_DestroyTexture(tex);
    mTextures.clear();
    IMG_Quit();
}

SDL_Texture* TextureManager::load(SDL_Renderer* renderer, const std::string& id, const std::string& path) {
    // Don't reload if already loaded
    auto it = mTextures.find(id);
    if (it != mTextures.end()) return it->second;

    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex) {
        SDL_Log("Failed to load texture '%s': %s", path.c_str(), IMG_GetError());
        return nullptr;
    }

    mTextures[id] = tex;
    return tex;
}

SDL_Texture* TextureManager::get(const std::string& id) const {
    auto it = mTextures.find(id);
    if (it != mTextures.end()) return it->second;
    return nullptr;
}

void TextureManager::unload(const std::string& id) {
    auto it = mTextures.find(id);
    if (it != mTextures.end()) {
        SDL_DestroyTexture(it->second);
        mTextures.erase(it);
    }
}