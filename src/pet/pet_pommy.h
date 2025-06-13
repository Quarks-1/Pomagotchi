#pragma once

#include <Arduino.h>
#include "main.h"

// Forward declarations
void logPet(uint8_t amount);

// Pet pommy state
extern bool isPetting;
extern uint8_t petFillLevel;

// Function declarations
void handlePetting(int8_t direction);
void togglePetting();
uint8_t getPetFillLevel();
void updatePettingDetection();
uint16_t getProximityLevel();
void setProximityThreshold(uint16_t threshold);
uint16_t getProximityThreshold(); 