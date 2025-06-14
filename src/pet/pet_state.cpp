#include "pet_state.h"
#include "depletion.h"
#include "storage/persistent_storage.h"
#include "activities/sunbathing.h"
#include "core/tasks.h"
#include "hardware/battery_monitor.h"
#include "core/main.h"
#include <Arduino.h>

// Initialize pet state variables
uint8_t sunlight = 100;  // Changed from hunger
uint8_t thirst = 100;
uint8_t petStatus = 0;   // 0-10 pets
uint32_t stars = 0;      // Star rewards
unsigned long lastUpdateTime = 0;
unsigned long lastStarCheckTime = 0;
constexpr unsigned long NORMAL_SUNLIGHT_UPDATE_INTERVAL = 60000; // Update every minute
constexpr unsigned long DEBUG_SUNLIGHT_UPDATE_INTERVAL = 100;   // Update every second in debug mode
constexpr unsigned long NORMAL_STAR_CHECK_INTERVAL = 86400000;  // Check once per day (24 hours)
constexpr unsigned long DEBUG_STAR_CHECK_INTERVAL = 10000;      // Check every 10 seconds in debug mode

void initializePetState() {
    // Load saved values from storage
    sunlight = loadSunlight();
    thirst = loadThirst();
    petStatus = loadPetStatus();
    stars = loadStars();
    
    // If values are 0 (uninitialized storage), set defaults
    if (sunlight == 0) sunlight = 100;
    if (thirst == 0) thirst = 100;
    if (petStatus == 0) petStatus = 10;
    
    // Initialize timers
    lastUpdateTime = millis();
    lastStarCheckTime = millis();
}

void updatePetState() {
    // This function is now handled by the logic task
    // It's kept for backward compatibility but is empty
}

uint8_t getBatteryLevel() {
    // Use the new battery monitor to get percentage
    return static_cast<uint8_t>(getBatteryPercent());
}

void checkAndGrantStars() {
    unsigned long currentTime = millis();
    unsigned long interval = isDebugModeActive() ? DEBUG_STAR_CHECK_INTERVAL : NORMAL_STAR_CHECK_INTERVAL;
    
    if (currentTime - lastStarCheckTime >= interval) {
        // Check if all stats are above 0
        if (sunlight > 0 && thirst > 0 && petStatus > 0) {
            Serial.println("Adding star");
            grantStar();
        }
        lastStarCheckTime = currentTime;
    }
}

void grantStar() {
    stars++;
    Serial.print("Star granted! Total stars: ");
    Serial.println(stars);
    saveStars(stars);
}

uint32_t getStars() {
    return stars;
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

void logPet(uint8_t amount) {
    Serial.print("Logging pets. Current pet status: ");
    Serial.print(petStatus);
    Serial.print(", Adding amount: ");
    Serial.println(amount);
    
    if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
        // Calculate new pet status value with bounds checking (0-10)
        int16_t newPetStatus = petStatus + amount;
        if (newPetStatus > 10) {
            petStatus = 10;
        } else if (newPetStatus < 0) {
            petStatus = 0;
        } else {
            petStatus = newPetStatus;
        }
        
        // Send storage event to save petStatus
        StorageEvent event = {StorageEvent::SAVE_PET_STATUS, petStatus};
        xQueueSend(storageQueue, &event, 0);
        
        xSemaphoreGive(petStateMutex);
    }
    
    Serial.print("New pet status: ");
    Serial.println(petStatus);
}

void resetPetStatus() {
    if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
        petStatus = 0;
        
        // Send storage event to save the reset value
        StorageEvent event = {StorageEvent::SAVE_PET_STATUS, petStatus};
        xQueueSend(storageQueue, &event, 0);
        
        xSemaphoreGive(petStateMutex);
    }
    Serial.println("Pet status reset to 0 - bar is now empty");
} 