#pragma once

#include <Arduino.h>
#include <LittleFS.h>

// File paths for our values
#define CONFIG_FILE "/pet_state.json"

// Function declarations
bool initializeStorage();
bool saveSunlight(uint8_t value);
bool saveThirst(uint8_t value);
bool savePetStatus(uint8_t value);
bool saveValues(uint8_t sunlight, uint8_t thirst, uint8_t petStatus, uint32_t stars);
uint8_t loadSunlight();
uint8_t loadThirst();
uint8_t loadPetStatus();

// Star reward functions
bool saveStars(uint32_t stars);
uint32_t loadStars();

// Hat functions
bool saveHatPurchased(uint8_t hatType, bool purchased);
bool saveHatWearing(uint8_t hatType, bool wearing);
bool loadHatPurchased(uint8_t hatType);
bool loadHatWearing(uint8_t hatType); 