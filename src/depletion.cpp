#include "depletion.h"

void updateDepletion(uint8_t& sunlight, uint8_t& thirst, unsigned long& lastUpdateTime) {
    unsigned long currentTime = millis();
    
    // Update sunlight level (1% every second)
    if (currentTime - lastUpdateTime >= SUNLIGHT_DEPLETION_INTERVAL) {
        if (sunlight > 0) sunlight--;
        lastUpdateTime = currentTime;
    }
    
    // Update thirst level (1% every 500ms)
    if (currentTime - lastUpdateTime >= THIRST_DEPLETION_INTERVAL) {
        if (thirst > 0) thirst--;
    }
} 