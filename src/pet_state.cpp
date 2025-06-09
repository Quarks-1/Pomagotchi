#include "pet_state.h"
#include "depletion.h"
#include "persistent_storage.h"
#include <Arduino.h>

// Initialize pet state variables
uint8_t sunlight = 100;  // Changed from hunger
uint8_t thirst = 100;
unsigned long lastUpdateTime = 0;

void initializePetState() {
    // Load saved values from storage
    sunlight = loadSunlight();
    thirst = loadThirst();
    
    // If values are 0 (uninitialized storage), set defaults
    if (sunlight == 0) sunlight = 100;
    if (thirst == 0) thirst = 100;
}

void updatePetState() {
    updateDepletion(sunlight, thirst, lastUpdateTime);
    
    // Save values periodically (every 5 seconds)
    static unsigned long lastSaveTime = 0;
    if (millis() - lastSaveTime >= 5000) {
        if (saveSunlight(sunlight) && saveThirst(thirst)) {
            lastSaveTime = millis();
        } else {
            Serial.println("Failed to save pet state");
        }
    }
}

uint8_t getBatteryLevel() {
    // Read battery voltage and convert to percentage
    float voltage = analogRead(A0) * (3.3 / 1023.0) * 2; // Assuming voltage divider
    uint8_t percentage = (voltage - 3.0) * 100 / (4.2 - 3.0); // LiPo battery range
    return constrain(percentage, 0, 100);
}

void logWater(uint8_t amount) {
    Serial.print("Logging water. Current thirst: ");
    Serial.print(thirst);
    Serial.print(", Adding amount: ");
    Serial.println(amount);
    
    // Calculate new thirst value with bounds checking
    int16_t newThirst = thirst + amount;
    if (newThirst > 100) {
        thirst = 100;
    } else if (newThirst < 0) {
        thirst = 0;
    } else {
        thirst = newThirst;
    }
    
    // Save the new thirst value immediately
    if (!saveThirst(thirst)) {
        Serial.println("Failed to save thirst value");
    }
    
    Serial.print("New thirst value: ");
    Serial.println(thirst);
} 