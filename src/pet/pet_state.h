#pragma once

#include <Arduino.h>

// Pet state variables
extern uint8_t sunlight;
extern uint8_t thirst;
extern uint8_t petStatus;  // 0-10 pets
extern uint32_t stars;
extern unsigned long lastUpdateTime;

// Logging flag variables for star rewards
extern uint8_t waterLoggedFlag;    // 1 if water has been logged since last star
extern uint8_t sunlightLoggedFlag; // 1 if sunlight has been logged since last star  
extern uint8_t petLoggedFlag;      // 1 if petting has been logged since last star

// Function declarations
void updatePetState();
void initializePetState();

// Get the current battery level (0-100)
uint8_t getBatteryLevel();

// Log water intake and update thirst level (0-100)
void logWater(uint8_t amount);

// Log pet interactions and update pet status (0-10)
void logPet(uint8_t amount);

// Log sunlight activity (called from sunbathing)
void logSunlight();

// Reset pet status to 0
void resetPetStatus();

// Star reward functions
void checkAndGrantStars();
void grantStar();
uint32_t getStars();
void adjustStarTimerAfterSleep(unsigned long sleepDurationMs);

// Logging flag functions
void checkAndRewardCompleteLogging();
void resetLoggingFlags(); 