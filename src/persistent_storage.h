#pragma once

#include <Arduino.h>
#include <LittleFS.h>

// File paths for our values
#define CONFIG_FILE "/pet_state.json"

// Function declarations
bool initializeStorage();
bool saveSunlight(uint8_t value);
bool saveThirst(uint8_t value);
bool saveValues(uint8_t sunlight, uint8_t thirst);
uint8_t loadSunlight();
uint8_t loadThirst(); 