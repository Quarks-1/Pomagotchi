#include "pet_state.h"
#include <Arduino.h>

// Initialize pet state variables
uint8_t sunlight = 100;  // Changed from hunger
uint8_t thirst = 100;
unsigned long lastUpdateTime = 0;

// Constants for depletion rates
const unsigned long SUNLIGHT_DEPLETION_INTERVAL = 1000;     // 1 second in milliseconds
// const unsigned long SUNLIGHT_DEPLETION_INTERVAL = 7200000;  // 2 hours in milliseconds (commented out for now)
const unsigned long THIRST_DEPLETION_INTERVAL = 500;        // 500ms for thirst

void updatePetState() {
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

uint8_t getBatteryLevel() {
    // Read battery voltage and convert to percentage
    float voltage = analogRead(A0) * (3.3 / 1023.0) * 2; // Assuming voltage divider
    uint8_t percentage = (voltage - 3.0) * 100 / (4.2 - 3.0); // LiPo battery range
    return constrain(percentage, 0, 100);
} 