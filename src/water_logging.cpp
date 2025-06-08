#include "screens.h"
#include "cursor.h"

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
        int16_t newLevel = waterFillLevel + direction;
        if (newLevel >= 0 && newLevel <= 100) {
            waterFillLevel = newLevel;
        }
    }
}

// Function to toggle water logging mode
void toggleWaterLogging() {
    isWaterLogging = !isWaterLogging;
    if (!isWaterLogging) {
        // Reset water fill level when exiting logging mode
        waterFillLevel = 0;
        // Reset cursor position to the up arrow (index 1)
        cursorPosition = 1;
        // Show logged message
        showLoggedMessage = true;
        loggedMessageStartTime = millis();
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