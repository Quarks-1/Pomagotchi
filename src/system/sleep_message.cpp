#include "sleep_message.h"

// Define the global variables
bool showSleepMessage = false;
unsigned long sleepMessageStartTime = 0;
const unsigned long SLEEP_MESSAGE_DURATION = 2000; // 2 seconds

// Function to check if sleep message should be shown
bool shouldShowSleepMessage() {
    if (showSleepMessage) {
        unsigned long currentTime = millis();
        if (currentTime - sleepMessageStartTime >= SLEEP_MESSAGE_DURATION) {
            showSleepMessage = false;
            return false;
        }
        return true;
    }
    return false;
}

// Function to reset sleep message state
void resetSleepMessage() {
    showSleepMessage = false;
    sleepMessageStartTime = 0;
}

// Function to show sleep message
void showSleepMessageNow() {
    showSleepMessage = true;
    sleepMessageStartTime = millis();
} 