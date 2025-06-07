#include "pet_state.h"

// Initialize pet state variables
uint8_t hunger = 100;
uint8_t thirst = 100;
unsigned long lastUpdateTime = 0;

void updatePetState() {
    unsigned long currentTime = millis();
    
    // Check if 500ms have passed
    if (currentTime - lastUpdateTime >= 500) {
        // Decrease hunger and thirst by 1%
        if (hunger > 0) hunger--;
        if (thirst > 0) thirst--;
        
        // Update the last update time
        lastUpdateTime = currentTime;
    }
} 