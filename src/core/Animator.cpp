#include "Animator.h"

Animator::Animator(int defaultFrameW, int defaultFrameH, int defaultFrames, float defaultDelay, int row)
    : frameWidth(defaultFrameW), frameHeight(defaultFrameH),
      totalFrames(defaultFrames), frameDelay(defaultDelay),
      mCurrentFrame(0), mCurrentRow(row), mFrameTimer(0.0f) {}

void Animator::addClip(const std::string& name, const AnimClip& clip) {
    mClips[name] = clip;
}

void Animator::play(const std::string& name) {
    if (name == mCurrentClip) return;  // already playing

    auto it = mClips.find(name);
    if (it == mClips.end()) return;    // clip not registered

    mCurrentClip  = name;
    const auto& clip = it->second;
    frameWidth    = clip.frameWidth;
    frameHeight   = clip.frameHeight;
    totalFrames   = clip.totalFrames;
    frameDelay    = clip.frameDelay;
    mLoop         = clip.loop;
    mCurrentFrame = 0;
    mFrameTimer   = 0.0f;
}

void Animator::update(float deltaTime) {
    mFrameTimer += deltaTime;
    if (mFrameTimer >= frameDelay) {
        mFrameTimer -= frameDelay;
        if (mLoop) {
            mCurrentFrame = (mCurrentFrame + 1) % totalFrames;
        } else {
            if (mCurrentFrame < totalFrames - 1)
                mCurrentFrame++;
        }
    }
}

SDL_Rect Animator::getSrcRect() const {
    return SDL_Rect{
        mCurrentFrame * frameWidth,
        mCurrentRow * frameHeight,
        frameWidth,
        frameHeight
    };
}

void Animator::setRow(int row) {
    if (row != mCurrentRow) {
        mCurrentRow = row;
        mCurrentFrame = 0;
        mFrameTimer = 0.0f;
    }
}

void Animator::reset() {
    mCurrentFrame = 0;
    mFrameTimer = 0.0f;
}