#include "pet_state.h"
#include "depletion.h"
#include "persistent_storage.h"
#include "sunbathing.h"
#include "tasks.h"
#include <Arduino.h>

// Initialize pet state variables
uint8_t sunlight = 100;  // Changed from hunger
uint8_t thirst = 100;
unsigned long lastUpdateTime = 0;
constexpr unsigned long NORMAL_SUNLIGHT_UPDATE_INTERVAL = 60000; // Update every minute
constexpr unsigned long DEBUG_SUNLIGHT_UPDATE_INTERVAL = 100;   // Update every second in debug mode

void initializePetState() {
    // Load saved values from storage
    sunlight = loadSunlight();
    thirst = loadThirst();
    
    // If values are 0 (uninitialized storage), set defaults
    if (sunlight == 0) sunlight = 100;
    if (thirst == 0) thirst = 100;
    
    // Initialize timer
    lastUpdateTime = millis();
}

void updatePetState() {
    // This function is now handled by the logic task
    // It's kept for backward compatibility but is empty
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
    
    if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
        // Calculate new thirst value with bounds checking
        int16_t newThirst = thirst + amount;
        if (newThirst > 100) {
            thirst = 100;
        } else if (newThirst < 0) {
            thirst = 0;
        } else {
            thirst = newThirst;
        }
        
        // Send storage event
        StorageEvent event = {StorageEvent::SAVE_THIRST, thirst};
        xQueueSend(storageQueue, &event, 0);
        
        xSemaphoreGive(petStateMutex);
    }
    
    Serial.print("New thirst value: ");
    Serial.println(thirst);
} 