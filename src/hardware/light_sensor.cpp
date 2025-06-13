#include "light_sensor.h"

// Create sensor object
static Adafruit_VCNL4020 vcnl4020;

bool initializeLightSensor() {
    if (!vcnl4020.begin()) {
        Serial.println("Couldn't find VCNL4020 chip");
        isLightSensorEnabled = false;
        return false;
    }
    
    // Configure sensor with lower sampling rates
    vcnl4020.setProxLEDmA(100);  // Reduced LED current to 100mA
    vcnl4020.setProxRate(PROX_RATE_1_95_PER_S);  // Reduced to ~2 measurements per second
    vcnl4020.setAmbientRate(AMBIENT_RATE_1_SPS);  // Reduced to 1 sample per second
    vcnl4020.setAmbientAveraging(AVG_1_SAMPLES);  // Single sample is sufficient
    
    // Enable continuous measurements
    vcnl4020.enable(true, true, true);  // Enable ambient, proximity, and self-timed mode
    
    isLightSensorEnabled = true;
    Serial.println("VCNL4020 initialized successfully");
    return true;
}

uint16_t getLightLevel() {
    if (!isLightSensorEnabled) {
        return 0;
    }
    return vcnl4020.readAmbient();  // Use readAmbient() for ambient light level
}

bool isInSunlight() {
    if (!isLightSensorEnabled) {
        return false;
    }
    uint16_t lightLevel = getLightLevel();
    return lightLevel >= MIN_SUNLIGHT_THRESHOLD && lightLevel <= MAX_SUNLIGHT_THRESHOLD;
}

uint16_t readProximityValue() {
    if (!isLightSensorEnabled) {
        return 0;
    }
    return vcnl4020.readProximity();  // Read proximity value from VCNL4020
}

void setLightSensorEnabled(bool enabled) {
    isLightSensorEnabled = enabled;
    if (enabled) {
        // Try to initialize the sensor when enabling
        initializeLightSensor();
    }
} 