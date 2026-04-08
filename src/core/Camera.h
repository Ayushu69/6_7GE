#pragma once
#include <SDL2/SDL.h>
#include "../Constants.h"

struct Camera{
    float x = 0.0f, y = 0.0f;
    float w, h;

    Camera(float screenW, float screenH) : w(screenW), h(screenH) {}

    void update(float targetCenterX, float targetCenterY) {
        x = targetCenterX - w * 0.5f;
        y = targetCenterY - h * 0.5f;

        //clamping to world bounds
        if(x < 0.0f) x = 0.0f;
        if(y < 0.0f) y = 0.0f;
        if(x + w > kWorldWidth) x = kWorldWidth - w;
        if(y + h > kWorldHeight) y = kWorldHeight - h;
    }
};