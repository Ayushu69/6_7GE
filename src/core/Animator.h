#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

// Describes one animation clip (e.g. "idle", "walk", "run")
struct AnimClip {
    int frameWidth  = 64;
    int frameHeight = 64;
    int totalFrames = 1;
    float frameDelay = 0.12f;  // seconds per frame
    bool loop = true;          // should the animation loop?
};

class Animator {
public:
    Animator() = default;
    Animator(int defaultFrameW, int defaultFrameH, int defaultFrames, float defaultDelay, int row = 0);

    // Register a named animation clip
    void addClip(const std::string& name, const AnimClip& clip);

    // Switch to a named clip (resets frame if clip changed)
    void play(const std::string& name);

    // Get the currently playing clip name
    const std::string& currentClipName() const { return mCurrentClip; }

    void update(float deltaTime);
    SDL_Rect getSrcRect() const;

    // Legacy API kept for compatibility
    void setRow(int row);
    void reset();

    // Public so Player can read them for sizing
    int frameWidth  = 64;
    int frameHeight = 64;
    int totalFrames = 1;
    float frameDelay = 0.12f;

private:
    std::unordered_map<std::string, AnimClip> mClips;
    std::string mCurrentClip;

    int mCurrentFrame = 0;
    int mCurrentRow   = 0;
    float mFrameTimer = 0.0f;
    bool mLoop = true;
};