#include "hat_sprites.h"

// Function to get hat sprite by type
const uint8_t* getHatSprite(uint8_t hatType) {
    switch (hatType) {
        case 1: return tophat_sprite;      // TOPHAT
        case 2: return cowboy_hat_sprite;  // COWBOY_HAT
        case 3: return party_hat_sprite;   // PARTY_HAT
        case 4: return star_hat_sprite;    // STAR_HAT
        default: return nullptr;
    }
} 