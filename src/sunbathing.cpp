#include "pet_state.h"
#include "screens.h"
#include "cursor.h"
#include "main.h"
#include "persistent_storage.h"
#include "light_sensor.h"

// Global variables for sunbathing state
bool isSunbathing = false;
uint8_t sunlightFillLevel = 0;
static unsigned long lastSunlightUpdate = 0;
constexpr unsigned long SUNLIGHT_UPDATE_INTERVAL = 60000; // Update every minute

// Function to handle sunbathing
void handleSunbathing(int8_t direction) {
    if (isSunbathing) {
        // Update sunlight fill level based on direction
        int16_t multiplier = isDebugModeActive() ? 20 : 1;  // Larger steps in debug mode
        int16_t newLevel = sunlightFillLevel + direction * multiplier;
        if (newLevel >= 0 && newLevel <= 100) {
            sunlightFillLevel = newLevel;
            Serial.print("Sunlight fill level updated: ");
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
        // Store the fill level before resetting
        uint8_t finalFillLevel = sunlightFillLevel;
        // Reset sunlight fill level when exiting logging mode
        sunlightFillLevel = 0;
        // Reset cursor position to the up arrow (index 1)
        cursorPosition = 1;
        // Update global sunlight variable
        sunlight = finalFillLevel;
        // Log the sunlight with the stored fill level
        saveSunlight(finalFillLevel);
        Serial.print("Updated global sunlight to: ");
        Serial.println(sunlight);
    } else {
        Serial.println("Entering sunbathing mode");
        lastSunlightUpdate = millis();
    }
}

// Function to get current sunlight fill level
uint8_t getSunlightFillLevel() {
    return sunlightFillLevel;
}

// Function to update sunlight level based on sensor readings
void updateSunlightLevel() {
    if (isSunbathing) {
        unsigned long currentTime = millis();
        if (currentTime - lastSunlightUpdate >= SUNLIGHT_UPDATE_INTERVAL) {
            if (isInSunlight()) {
                // Increment sunlight level by 1% every minute when in sunlight
                if (sunlightFillLevel < 100) {
                    sunlightFillLevel++;
                    Serial.print("Sunlight level increased to: ");
                    Serial.println(sunlightFillLevel);
                }
            }
            lastSunlightUpdate = currentTime;
        }
    }
} 