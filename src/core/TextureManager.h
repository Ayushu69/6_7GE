#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <string>

class TextureManager {
public:
    // Call once at startup
    bool init();
    // Call once at shutdown
    void shutdown();

    // Load a texture from path, store it under 'id', return the pointer
    SDL_Texture* load(SDL_Renderer* renderer, const std::string& id, const std::string& path);

    // Get a previously loaded texture by id
    SDL_Texture* get(const std::string& id) const;

    // Free a specific texture
    void unload(const std::string& id);

private:
    std::unordered_map<std::string, SDL_Texture*> mTextures;
};