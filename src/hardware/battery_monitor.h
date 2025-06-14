#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LC709203F.h>

// Function declarations
bool initializeBatteryMonitor();
float getBatteryVoltage();
float getBatteryPercent();
void setBatteryMonitorEnabled(bool enabled);

// Battery monitor state
extern bool isBatteryMonitorEnabled; 