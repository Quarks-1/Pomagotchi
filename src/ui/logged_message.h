#pragma once

#include <Arduino.h>

// Global variables for logged message state
extern bool showLoggedMessage;
extern unsigned long loggedMessageStartTime;
extern const unsigned long LOGGED_MESSAGE_DURATION;

// Function declarations
bool shouldShowLoggedMessage();
void resetLoggedMessage();
void showLoggedMessageNow(); 