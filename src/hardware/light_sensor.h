#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VCNL4020.h>

// Light sensor thresholds
constexpr uint16_t MIN_SUNLIGHT_THRESHOLD = 1000;  // Minimum light level to consider as sunlight
constexpr uint16_t MAX_SUNLIGHT_THRESHOLD = 65535; // Maximum light level from sensor

// Light sensor state
extern bool isLightSensorEnabled;

// Function declarations
bool initializeLightSensor();
uint16_t getLightLevel();
bool isInSunlight();
uint16_t readProximityValue();
void setLightSensorEnabled(bool enabled); 