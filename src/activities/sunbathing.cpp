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
static unsigned long lastSunlightUpdate = 0;  // Track last update time

// Sleep-related sunbathing state
static bool wasSunbathingBeforeSleepFlag = false;

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

// Sleep-related sunbathing functions
void applySunbathingDuringSleep(unsigned long sleepDurationMs) {
    if (wasSunbathingBeforeSleepFlag) {
        // Calculate how many minutes passed during sleep
        unsigned long sleepMinutes = sleepDurationMs / 60000; // Convert ms to minutes
        
        // Add sunlight progress for each minute of sleep (assuming in sunlight)
        // This is simplified - in reality we'd need to check light sensor history
        uint8_t progressToAdd = min((uint8_t)sleepMinutes, (uint8_t)(100 - sunlightFillLevel));
        
        if (progressToAdd > 0) {
            sunlightFillLevel += progressToAdd;
            Serial.print("Applied sunbathing progress during sleep: +");
            Serial.print(progressToAdd);
            Serial.print("%, new fill level: ");
            Serial.println(sunlightFillLevel);
        }
    }
}

bool wasSunbathingBeforeSleep() {
    return wasSunbathingBeforeSleepFlag;
}

void setSunbathingStateForSleep(bool wasSunbathing) {
    wasSunbathingBeforeSleepFlag = wasSunbathing;
    Serial.print("Sunbathing state saved for sleep: ");
    Serial.println(wasSunbathing ? "was sunbathing" : "was not sunbathing");
} 