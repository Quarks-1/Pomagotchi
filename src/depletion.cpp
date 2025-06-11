#include "depletion.h"

void updateDepletion(uint8_t& sunlight, uint8_t& thirst, unsigned long& lastUpdateTime) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - lastUpdateTime;
    
    // Update sunlight level (1% every second)
    if (elapsedTime >= SUNLIGHT_DEPLETION_INTERVAL) {
        if (sunlight > 0) sunlight--;
        lastUpdateTime = currentTime;
    }
    
    // Update thirst level (1% every 500ms)
    // Check if we've passed multiple 500ms intervals
    unsigned long thirstIntervals = elapsedTime / THIRST_DEPLETION_INTERVAL;
    if (thirstIntervals > 0) {
        if (thirst > thirstIntervals) {
            thirst -= thirstIntervals;
        } else {
            thirst = 0;
        }
    }
} 