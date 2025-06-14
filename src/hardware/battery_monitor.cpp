#include "battery_monitor.h"

// Create battery monitor object
static Adafruit_LC709203F lc;

// Battery monitor state
bool isBatteryMonitorEnabled = true;

bool initializeBatteryMonitor() {
    if (!lc.begin()) {
        Serial.println("Couldn't find LC709203F sensor");
        isBatteryMonitorEnabled = false;
        return false;
    }
    
    // Configure for 3.7V LiPo battery
    lc.setPackSize(LC709203F_APA_500MAH);
    
    isBatteryMonitorEnabled = true;
    Serial.println("LC709203F battery monitor initialized successfully");
    return true;
}

float getBatteryVoltage() {
    if (!isBatteryMonitorEnabled) {
        return 0.0f;
    }
    return lc.cellVoltage();
}

float getBatteryPercent() {
    if (!isBatteryMonitorEnabled) {
        return 0.0f;
    }
    return lc.cellPercent();
}

void setBatteryMonitorEnabled(bool enabled) {
    if (enabled && !isBatteryMonitorEnabled) {
        // Try to initialize when enabling
        initializeBatteryMonitor();
    } else {
        isBatteryMonitorEnabled = enabled;
    }
} 