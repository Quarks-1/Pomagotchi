#include "pet/pet_state.h"
#include "ui/screens/screens.h"
#include "ui/cursor.h"
#include "core/main.h"
#include "storage/persistent_storage.h"
#include "hardware/light_sensor.h"
#include "ui/logged_message.h"

// Global variables for sunbathing state
bool isSunbathing = false;
uint8_t sunlightFillLevel = 0;
constexpr unsigned long SUNLIGHT_UPDATE_INTERVAL = 60000; // Update every minute

// Function to handle sunbathing
void handleSunbathing(int8_t direction) {
    if (isSunbathing) {
        // Update sunlight fill level based on direction
        int16_t multiplier = isDebugModeActive() ? 20 : 1;  // Larger steps in debug mode
        int16_t newLevel = sunlightFillLevel + direction * multiplier;
        if (newLevel >= 0 && newLevel <= 100) {
            sunlightFillLevel = newLevel;
            Serial.print("Sunlight fill level updated by encoder: ");
            Serial.println(sunlightFillLevel);
        }
    }
}

// Function to toggle sunbathing mode
void toggleSunbathing() {
    isSunbathing = !isSunbathing;
    if (!isSunbathing) {
        Serial.print("Exiting sunbathing mode. Final fill level: ");
        Serial.println(sunlightFillLevel);
        
        // Add the fill level to current sunlight level
        int16_t newSunlight = sunlight + sunlightFillLevel;
        if (newSunlight > 100) {
            sunlight = 100;
        } else if (newSunlight < 0) {
            sunlight = 0;
        } else {
            sunlight = newSunlight;
        }
        
        // Reset sunlight fill level when exiting logging mode
        sunlightFillLevel = 0;
        // Reset cursor position to the up arrow (index 1)
        cursorPosition = 1;
        
        // Save the new sunlight value
        if (!saveSunlight(sunlight)) {
            Serial.println("Failed to save sunlight value");
        }
        
        // Show logged message using shared function
        resetLoggedMessage();  // Reset first to ensure clean state
        showLoggedMessageNow();
        
        Serial.print("Updated global sunlight to: ");
        Serial.println(sunlight);
    } else {
        Serial.println("Entering sunbathing mode");
    }
}

// Function to get current sunlight fill level
uint8_t getSunlightFillLevel() {
    return sunlightFillLevel;
}

// Function to update sunlight level based on sensor readings
void updateSunlightLevel() {
    if (isSunbathing) {
        if (isInSunlight()) {
            // Increment sunlight level by 1% every minute when in sunlight
            if (sunlightFillLevel < 100) {
                sunlightFillLevel++;
                Serial.print("Sunlight level increased to: ");
                Serial.println(sunlightFillLevel);
            }
        }
    }
} 