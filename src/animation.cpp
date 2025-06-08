#include "animation.h"
#include "sprites.h"

namespace {
    // Idle animation frames (both idle1 and idle2)
    const AnimationFrame idleFrames[] = {
        {idle1_0, idle1_0[0], idle1_0[1]},
        {idle1_1, idle1_1[0], idle1_1[1]},
        {idle1_2, idle1_2[0], idle1_2[1]},
        {idle1_3, idle1_3[0], idle1_3[1]},
        {idle1_4, idle1_4[0], idle1_4[1]},
        {idle2_0, idle2_0[0], idle2_0[1]},
        {idle2_1, idle2_1[0], idle2_1[1]},
        {idle2_2, idle2_2[0], idle2_2[1]},
        {idle2_3, idle2_3[0], idle2_3[1]},
        {idle2_4, idle2_4[0], idle2_4[1]},
    };

    // Jump animation frames (all 12 frames)
    const AnimationFrame jumpFrames[] = {
        {jump_0, jump_0[0], jump_0[1]},
        {jump_1, jump_1[0], jump_1[1]},
        {jump_2, jump_2[0], jump_2[1]},
        {jump_3, jump_3[0], jump_3[1]},
        {jump_4, jump_4[0], jump_4[1]},
        {jump_5, jump_5[0], jump_5[1]},
        {jump_6, jump_6[0], jump_6[1]},
        {jump_7, jump_7[0], jump_7[1]},
        {jump_8, jump_8[0], jump_8[1]},
        {jump_9, jump_9[0], jump_9[1]},
        {jump_10, jump_10[0], jump_10[1]},
    };

    // Sit animation frames (all 9 frames)
    const AnimationFrame sitFrames[] = {
        {sit_0, sit_0[0], sit_0[1]},
        {sit_1, sit_1[0], sit_1[1]},
        {sit_2, sit_2[0], sit_2[1]},
        {sit_3, sit_3[0], sit_3[1]},
        {sit_4, sit_4[0], sit_4[1]},
        {sit_5, sit_5[0], sit_5[1]},
        {sit_6, sit_6[0], sit_6[1]},
        {sit_7, sit_7[0], sit_7[1]},
        {sit_8, sit_8[0], sit_8[1]}
    };

    // Walk animation frames (all 5 frames)
    const AnimationFrame walkFrames[] = {
        {walk_0, walk_0[0], walk_0[1]},
        {walk_1, walk_1[0], walk_1[1]},
        {walk_2, walk_2[0], walk_2[1]},
        {walk_3, walk_3[0], walk_3[1]},
        {walk_4, walk_4[0], walk_4[1]}
    };

    // Sniff animation frames (all 8 frames)
    const AnimationFrame sniffFrames[] = {
        {sniff_0, sniff_0[0], sniff_0[1]},
        {sniff_1, sniff_1[0], sniff_1[1]},
        {sniff_2, sniff_2[0], sniff_2[1]},
        {sniff_3, sniff_3[0], sniff_3[1]},
        {sniff_4, sniff_4[0], sniff_4[1]},
        {sniff_5, sniff_5[0], sniff_5[1]},
        {sniff_6, sniff_6[0], sniff_6[1]},
        {sniff_7, sniff_7[0], sniff_7[1]}
    };

    // Run animation frames (all 8 frames)
    const AnimationFrame runFrames[] = {
        {run_0, run_0[0], run_0[1]},
        {run_1, run_1[0], run_1[1]},
        {run_2, run_2[0], run_2[1]},
        {run_3, run_3[0], run_3[1]},
        {run_4, run_4[0], run_4[1]},
        {run_5, run_5[0], run_5[1]},
        {run_6, run_6[0], run_6[1]},
        {run_7, run_7[0], run_7[1]}
    };

    // Sniff and walk animation frames (all 8 frames)
    const AnimationFrame sniffAndWalkFrames[] = {
        {sniff_and_walk_0, sniff_and_walk_0[0], sniff_and_walk_0[1]},
        {sniff_and_walk_1, sniff_and_walk_1[0], sniff_and_walk_1[1]},
        {sniff_and_walk_2, sniff_and_walk_2[0], sniff_and_walk_2[1]},
        {sniff_and_walk_3, sniff_and_walk_3[0], sniff_and_walk_3[1]},
        {sniff_and_walk_4, sniff_and_walk_4[0], sniff_and_walk_4[1]},
        {sniff_and_walk_5, sniff_and_walk_5[0], sniff_and_walk_5[1]},
        {sniff_and_walk_6, sniff_and_walk_6[0], sniff_and_walk_6[1]},
        {sniff_and_walk_7, sniff_and_walk_7[0], sniff_and_walk_7[1]}
    };

    const int animationDelay = 25; 
    // Animation sequences
    const AnimationSequence animations[] = {
        {idleFrames, 10, animationDelay},     // 12 frames for idle
        {jumpFrames, 11, animationDelay},     // 12 frames for jump
        {sitFrames, 9, animationDelay},       // 9 frames for sit
        {walkFrames, 5, animationDelay},      // 5 frames for walk
        {sniffFrames, 8, animationDelay},     // 8 frames for sniff
        {runFrames, 8, animationDelay},       // 8 frames for run
        {sniffAndWalkFrames, 8, animationDelay}  // 8 frames for sniff and walk
    };
}

const AnimationSequence& getAnimationSequence(AnimationType type) {
    static const AnimationSequence& idle = animations[0];
    static const AnimationSequence& jump = animations[1];
    static const AnimationSequence& sit = animations[2];
    static const AnimationSequence& walk = animations[3];
    static const AnimationSequence& sniff = animations[4];
    static const AnimationSequence& run = animations[5];
    static const AnimationSequence& sniffAndWalk = animations[6];

    switch (type) {
        case AnimationType::IDLE:
            return idle;
        case AnimationType::JUMP:
            return jump;
        case AnimationType::SIT:
            return sit;
        case AnimationType::WALK:
            return walk;
        case AnimationType::SNIFF:
            return sniff;
        case AnimationType::RUN:
            return run;
        case AnimationType::SNIFF_AND_WALK:
            return sniffAndWalk;
        default:
            return idle;  // Default to idle animation
    }
}

void updateAnimation(AnimationState& state) {
    const AnimationSequence& sequence = getAnimationSequence(state.currentType);
    unsigned long now = millis();
    if (now - state.lastFrameTime >= sequence.frameDelay) {
        static bool forward = true;  // Direction flag
        
        if (state.loop) {
            // Back-and-forth motion
            if (forward) {
                state.currentFrame++;
                if (state.currentFrame >= sequence.frameCount - 1) {
                    forward = false;
                }
            } else {
                state.currentFrame--;
                if (state.currentFrame <= 0) {
                    forward = true;
                }
            }
        } else {
            // Normal looping
            state.currentFrame = (state.currentFrame + 1) % sequence.frameCount;
        }
        state.lastFrameTime = now;
    }
}

const AnimationFrame& getCurrentFrame(const AnimationState& state) {
    const AnimationSequence& sequence = getAnimationSequence(state.currentType);
    return sequence.frames[state.currentFrame];
}

void changeAnimation(AnimationState& state, AnimationType newType) {
    if (state.currentType != newType) {
        state.currentType = newType;
        state.currentFrame = 0;
        state.lastFrameTime = millis();
    }
} 