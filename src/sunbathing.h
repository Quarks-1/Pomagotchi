#pragma once

#include <Arduino.h>
#include "main.h"

// Forward declarations
void logSunlight(uint8_t amount);

// Sunbathing state
extern bool isSunbathing;
extern uint8_t sunlightFillLevel;

// Function declarations
void handleSunbathing(int8_t direction);
void toggleSunbathing();
uint8_t getSunlightFillLevel();
void updateSunlightLevel(); 