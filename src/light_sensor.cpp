#include "light_sensor.h"

// Create sensor object
static Adafruit_VCNL4040 vcnl4040;


bool initializeLightSensor() {
    if (!vcnl4040.begin()) {
        Serial.println("Couldn't find VCNL4040 chip");
        isLightSensorEnabled = false;
        return false;
    }
    
    // Configure sensor
    vcnl4040.setProximityLEDCurrent(VCNL4040_LED_CURRENT_200MA);
    vcnl4040.setProximityLEDDutyCycle(VCNL4040_LED_DUTY_1_40);
    vcnl4040.setAmbientIntegrationTime(VCNL4040_AMBIENT_INTEGRATION_TIME_80MS);
    vcnl4040.setProximityIntegrationTime(VCNL4040_PROXIMITY_INTEGRATION_TIME_8T);
    
    isLightSensorEnabled = true;
    Serial.println("VCNL4040 initialized successfully");
    return true;
}

uint16_t getLightLevel() {
    if (!isLightSensorEnabled) {
        return 0;
    }
    return vcnl4040.getLux();
}

bool isInSunlight() {
    if (!isLightSensorEnabled) {
        return false;
    }
    uint16_t lightLevel = getLightLevel();
    return lightLevel >= MIN_SUNLIGHT_THRESHOLD && lightLevel <= MAX_SUNLIGHT_THRESHOLD;
}

void setLightSensorEnabled(bool enabled) {
    isLightSensorEnabled = enabled;
    if (enabled) {
        // Try to initialize the sensor when enabling
        initializeLightSensor();
    }
} 