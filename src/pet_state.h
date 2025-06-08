#pragma once

#include <Arduino.h>

// Pet state variables
extern uint8_t sunlight;
extern uint8_t thirst;
extern unsigned long lastUpdateTime;

// Function declarations
void updatePetState();

// Get the current battery level (0-100)
uint8_t getBatteryLevel(); 