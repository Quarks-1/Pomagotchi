#pragma once

#include <Arduino.h>
#include "main.h"

// Forward declarations
void logWater(uint8_t amount);

// Water logging state
extern bool isWaterLogging;
extern uint8_t waterFillLevel;

// Function declarations
void handleWaterLogging(int8_t direction);
void toggleWaterLogging();
uint8_t getWaterFillLevel(); 