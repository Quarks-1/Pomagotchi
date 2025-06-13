#pragma once

#include <Arduino.h>

// Global variables for sleep message state
extern bool showSleepMessage;
extern unsigned long sleepMessageStartTime;
extern const unsigned long SLEEP_MESSAGE_DURATION;

// Function declarations
bool shouldShowSleepMessage();
void resetSleepMessage();
void showSleepMessageNow(); 