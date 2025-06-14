#include "pet_state.h"
#include "ui/screens/screens.h"
#include "ui/cursor.h"
#include "core/main.h"
#include "ui/logged_message.h"
#include "hardware/light_sensor.h"

// Global variables for pet pommy state
bool isPetting = false;
uint8_t petFillLevel = 0;

// Proximity sensor variables for petting detection
static uint16_t lastProximityValue = 0;
static bool objectDetected = false;
static unsigned long pettingStartTime = 0;
static unsigned long lastPetTime = 0;
static uint16_t PROXIMITY_THRESHOLD = 4000;  // Threshold for object detection (adjustable)
constexpr unsigned long PET_COOLDOWN = 1000;    // 1 second cooldown between pets
constexpr unsigned long PET_TIMEOUT = 2000;     // 2 seconds max for a petting gesture

// Function to get proximity level from VCNL4020 sensor
uint16_t getProximityLevel() {
    if (!isLightSensorEnabled) {
        return 0;
    }
    // Access the static vcnl4020 object through a dedicated function
    // We'll need to add this to light_sensor.cpp
    extern uint16_t readProximityValue();
    return readProximityValue();
}

// Function to handle petting (manual adjustment via encoder)
void handlePetting(int8_t direction) {
    if (isPetting) {
        // Update pet fill level based on direction (0-10 scale, so multiply by 10 for percentage)
        int16_t multiplier = isDebugModeActive() ? 2 : 1;  // Larger steps in debug mode
        int16_t newLevel = petFillLevel + direction * multiplier;
        if (newLevel >= 0 && newLevel <= 10) {
            petFillLevel = newLevel;
            Serial.print("Pet fill level updated by encoder: ");
            Serial.println(petFillLevel);
        }
    }
}

// Function to detect petting gestures using proximity sensor
void updatePettingDetection() {
    if (!isPetting) {
        return;  // Only detect petting when in petting mode
    }
    
    unsigned long currentTime = millis();
    uint16_t proximityValue = getProximityLevel();
    
    // DEBUG: Print proximity values every few seconds when in pet mode
    static unsigned long lastDebugPrint = 0;
    if (currentTime - lastDebugPrint > 2000) {  // Print every 2 seconds
        Serial.print("PET MODE - Proximity: ");
        Serial.print(proximityValue);
        Serial.print(", Threshold: ");
        Serial.print(PROXIMITY_THRESHOLD);
        Serial.print(", Detected: ");
        Serial.println(objectDetected ? "YES" : "NO");
        lastDebugPrint = currentTime;
    }
    
    // Check for object approaching (proximity value increases)
    if (!objectDetected && proximityValue > PROXIMITY_THRESHOLD) {
        objectDetected = true;
        pettingStartTime = currentTime;
        Serial.print("Object detected! Proximity: ");
        Serial.println(proximityValue);
    }
    // Check for object leaving (proximity value decreases after being detected)
    else if (objectDetected && proximityValue < PROXIMITY_THRESHOLD) {
        unsigned long gestureTime = currentTime - pettingStartTime;
        
        Serial.print("Object left! Gesture time: ");
        Serial.print(gestureTime);
        Serial.print("ms, Cooldown OK: ");
        Serial.println((currentTime - lastPetTime) > PET_COOLDOWN ? "YES" : "NO");
        
        // Valid petting gesture: object was detected and then left within timeout
        if (gestureTime < PET_TIMEOUT && (currentTime - lastPetTime) > PET_COOLDOWN) {
            if (petFillLevel < 10) {
                petFillLevel++;
                lastPetTime = currentTime;
                Serial.print("*** PET DETECTED! Fill level: ");
                Serial.println(petFillLevel);
                
                // No immediate logged message - only show when exiting pet mode
            } else {
                Serial.println("Pet fill level already at max (10)");
            }
        } else {
            Serial.println("Invalid petting gesture (too slow or too fast)");
        }
        
        objectDetected = false;
    }
    // Reset if object stays too long (not a petting gesture)
    else if (objectDetected && (currentTime - pettingStartTime) > PET_TIMEOUT) {
        objectDetected = false;
        Serial.println("Petting gesture timeout - resetting");
    }
    
    lastProximityValue = proximityValue;
}

// Function to toggle petting mode
void togglePetting() {
    isPetting = !isPetting;
    if (!isPetting) {
        Serial.print("Exiting petting mode. Final pet level: ");
        Serial.println(petFillLevel);
        // Store the fill level before resetting
        uint8_t finalPetLevel = petFillLevel;
        // Reset pet fill level when exiting petting mode
        petFillLevel = 0;
        // Reset cursor position to the up arrow (index 1)
        cursorPosition = 1;
        // Log the pets with the stored fill level
        logPet(finalPetLevel);
        // Show logged message ONLY when exiting pet mode
        showLoggedMessageNow();
        
        // Reset detection state
        objectDetected = false;
        lastProximityValue = 0;
    } else {
        Serial.println("Entering petting mode");
        // Reset logged message state when entering petting mode
        resetLoggedMessage();
        
        // Initialize proximity detection
        objectDetected = false;
        lastProximityValue = getProximityLevel();
        lastPetTime = millis() - PET_COOLDOWN;  // Allow immediate petting
    }
}

// Function to get current pet fill level
uint8_t getPetFillLevel() {
    return petFillLevel;
}

// Function to set proximity threshold for debugging
void setProximityThreshold(uint16_t threshold) {
    PROXIMITY_THRESHOLD = threshold;
    Serial.print("Proximity threshold set to: ");
    Serial.println(threshold);
}

// Function to get current proximity threshold
uint16_t getProximityThreshold() {
    return PROXIMITY_THRESHOLD;
} 