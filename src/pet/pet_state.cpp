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

// Logging flag variables for star rewards
uint8_t waterLoggedFlag = 0;    // 1 if water has been logged since last star
uint8_t sunlightLoggedFlag = 0; // 1 if sunlight has been logged since last star  
uint8_t petLoggedFlag = 0;      // 1 if petting has been logged since last star

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
    
    // Load logging flags
    waterLoggedFlag = loadWaterLoggedFlag();
    sunlightLoggedFlag = loadSunlightLoggedFlag();
    petLoggedFlag = loadPetLoggedFlag();
    
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
            // Only reset timer when star is actually granted
            lastStarCheckTime = currentTime;
        }
        // If stats are not all > 0, don't reset timer - keep checking until they are
    }
    
    // Also check for complete logging reward (independent of daily timer)
    checkAndRewardCompleteLogging();
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
        
        // Set water logged flag
        if (!waterLoggedFlag) {
            waterLoggedFlag = 1;
            saveWaterLoggedFlag(waterLoggedFlag);
            Serial.println("Water logged flag set!");
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
        
        // Set pet logged flag
        if (!petLoggedFlag) {
            petLoggedFlag = 1;
            savePetLoggedFlag(petLoggedFlag);
            Serial.println("Pet logged flag set!");
        }
        
        // Send storage event to save petStatus
        StorageEvent event = {StorageEvent::SAVE_PET_STATUS, petStatus};
        xQueueSend(storageQueue, &event, 0);
        
        xSemaphoreGive(petStateMutex);
    }
    
    Serial.print("New pet status: ");
    Serial.println(petStatus);
}

void logSunlight() {
    Serial.println("Logging sunlight activity");
    
    // Set sunlight logged flag
    if (!sunlightLoggedFlag) {
        sunlightLoggedFlag = 1;
        saveSunlightLoggedFlag(sunlightLoggedFlag);
        Serial.println("Sunlight logged flag set!");
    }
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

// Function to adjust star timer after sleep (called from sleep manager)
void adjustStarTimerAfterSleep(unsigned long sleepDurationMs) {
    // Advance the star check timer by the amount of sleep time
    // This ensures sleep time counts toward the daily star interval
    lastStarCheckTime += sleepDurationMs;
    
    Serial.print("Adjusted star timer after sleep by ");
    Serial.print(sleepDurationMs / 1000);
    Serial.println(" seconds");
}

// Check if all three activities have been logged and reward with a star
void checkAndRewardCompleteLogging() {
    if (waterLoggedFlag && sunlightLoggedFlag && petLoggedFlag) {
        Serial.println("All three activities logged! Granting bonus star!");
        grantStar();
        resetLoggingFlags();
    }
}

// Reset all logging flags to 0 and save them
void resetLoggingFlags() {
    waterLoggedFlag = 0;
    sunlightLoggedFlag = 0;
    petLoggedFlag = 0;
    
    // Save all flags to storage
    saveLoggingFlags(waterLoggedFlag, sunlightLoggedFlag, petLoggedFlag);
    Serial.println("All logging flags reset to 0");
} 