#pragma once
#include <SDL2/SDL.h>
#include "../Constants.h"
#include "../entities/Player.h"

struct Camera{
    float x = 0.0f, y = 0.0f;
    float w, h;

    Camera(float screenW, float screenH) : w(screenW), h(screenH) {}

    void update(const Player& player){
        x = player.rect.x + player.rect.w*0.5f - w*0.5f;
        y = player.rect.y + player.rect.h*0.5f - h*0.5f;

        //clamping to world bounds
        if(x < 0.0f) x = 0.0f;
        if(y < 0.0f) y = 0.0f;
        if(x + w > kWorldWidth) x = kWorldWidth - w;
        if(y + h > kWorldHeight) y = kWorldHeight - h;
    }
};