#pragma once

#include <Arduino.h>

// Pet state variables
extern uint8_t sunlight;
extern uint8_t thirst;
extern unsigned long lastUpdateTime;

// Function declarations
void updatePetState();
void initializePetState();

// Get the current battery level (0-100)
uint8_t getBatteryLevel();

// Log water intake and update thirst level (0-100)
void logWater(uint8_t amount); 