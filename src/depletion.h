#pragma once

#include <Arduino.h>

// Constants for depletion rates
constexpr unsigned long SUNLIGHT_DEPLETION_INTERVAL = 2000;     // 1 second in milliseconds
// constexpr unsigned long SUNLIGHT_DEPLETION_INTERVAL = 7200000;  // 2 hours in milliseconds (commented out for now)
constexpr unsigned long THIRST_DEPLETION_INTERVAL = 2000;        // 500ms for thirst
constexpr unsigned long PET_STATUS_DEPLETION_INTERVAL = 7200000;  // 2 hours in milliseconds

// Function declarations
void updateDepletion(uint8_t& sunlight, uint8_t& thirst, uint8_t& petStatus, unsigned long& lastUpdateTime); 