#pragma once

#include <Arduino.h>
#include <stdint.h>

// Animation types
enum class AnimationType {
    IDLE,
    JUMP,
    SIT,
    WALK,
    SNIFF,
    RUN,
    SNIFF_AND_WALK
};

// Animation frame structure
struct AnimationFrame {
    const uint8_t* data;
    int width;
    int height;
};

// Animation sequence structure
struct AnimationSequence {
    const AnimationFrame* frames;
    int frameCount;
    int frameDelay;  // ms per frame
};

// Animation state structure
struct AnimationState {
    AnimationType currentType;
    int currentFrame;
    unsigned long lastFrameTime;
    bool loop;  // Controls whether animation plays back-and-forth
};

// Function declarations
const AnimationSequence& getAnimationSequence(AnimationType type);
void updateAnimation(AnimationState& state);
const AnimationFrame& getCurrentFrame(const AnimationState& state);
void changeAnimation(AnimationState& state, AnimationType newType); 