#include "pet_state.h"
#include "screens.h"
#include "cursor.h"
#include "main.h"

// Global variables for water logging state
bool isWaterLogging = false;
static uint8_t waterFillLevel = 0;
static bool showLoggedMessage = false;
static unsigned long loggedMessageStartTime = 0;
const unsigned long LOGGED_MESSAGE_DURATION = 2000; // 2 seconds

// Function to handle water logging
void handleWaterLogging(int8_t direction) {
    if (isWaterLogging) {
        // Update water fill level based on direction
        int16_t multiplier = isDebugModeActive() ? 20 : 1;  // Larger steps in debug mode
        int16_t newLevel = waterFillLevel + direction * multiplier;
        if (newLevel >= 0 && newLevel <= 100) {
            waterFillLevel = newLevel;
            Serial.print("Water fill level updated: ");
            Serial.println(waterFillLevel);
        }
    }
}

// Function to toggle water logging mode
void toggleWaterLogging() {
    isWaterLogging = !isWaterLogging;
    if (!isWaterLogging) {
        Serial.print("Exiting water logging mode. Final fill level: ");
        Serial.println(waterFillLevel);
        // Store the fill level before resetting
        uint8_t finalFillLevel = waterFillLevel;
        // Reset water fill level when exiting logging mode
        waterFillLevel = 0;
        // Reset cursor position to the up arrow (index 1)
        cursorPosition = 1;
        // Show logged message
        showLoggedMessage = true;
        loggedMessageStartTime = millis();
        // Log the water with the stored fill level
        logWater(finalFillLevel);
    } else {
        Serial.println("Entering water logging mode");
    }
}

// Function to get current water fill level
uint8_t getWaterFillLevel() {
    return waterFillLevel;
}

// Function to check if logged message should be shown
bool shouldShowLoggedMessage() {
    if (showLoggedMessage) {
        if (millis() - loggedMessageStartTime >= LOGGED_MESSAGE_DURATION) {
            showLoggedMessage = false;
            return false;
        }
        return true;
    }
    return false;
}

// Function to reset logged message state
void resetLoggedMessage() {
    showLoggedMessage = false;
} 