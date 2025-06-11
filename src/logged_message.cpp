#include "logged_message.h"

// Define the global variables
bool showLoggedMessage = false;
unsigned long loggedMessageStartTime = 0;
const unsigned long LOGGED_MESSAGE_DURATION = 2000; // 2 seconds

// Function to check if logged message should be shown
bool shouldShowLoggedMessage() {
    if (showLoggedMessage) {
        unsigned long currentTime = millis();
        if (currentTime - loggedMessageStartTime >= LOGGED_MESSAGE_DURATION) {
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
    loggedMessageStartTime = 0;
}

// Function to show logged message
void showLoggedMessageNow() {
    showLoggedMessage = true;
    loggedMessageStartTime = millis();
} 