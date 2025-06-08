#pragma once

#include <Arduino.h>

// Water logging state
extern bool isWaterLogging;
extern uint8_t waterFillLevel;

// Function declarations
void handleWaterLogging(int8_t direction);
void toggleWaterLogging();
uint8_t getWaterFillLevel();
bool shouldShowLoggedMessage();
void resetLoggedMessage(); 