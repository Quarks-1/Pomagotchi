#pragma once

#include <Arduino.h>

// Pet state variables
extern uint8_t hunger;
extern uint8_t thirst;
extern unsigned long lastUpdateTime;

// Function declarations
void updatePetState(); 