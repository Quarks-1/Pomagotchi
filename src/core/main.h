#pragma once

#include <Arduino.h>
#include "encoder.h"
#include "screens.h"

// Global variables
extern Encoder encoder;
extern bool isLightSensorEnabled;
extern bool isDebugMode;

// Function declarations
void handleSerialInput(char c);
void updateDisplay();
void changePage(Page newPage);
bool isDebugModeActive(); 