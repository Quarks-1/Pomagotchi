#include "store_purchasing.h"
#include "pet/pet_state.h"
#include "ui/screens/screens.h"
#include "ui/cursor.h"
#include "core/main.h"
#include "ui/logged_message.h"
#include "storage/persistent_storage.h"
#include "core/tasks.h"

// Global variables for store purchasing state
bool isPurchasing = false;
uint8_t selectedHat = 1; // Start with Tophat selection

// Function to handle hat selection navigation (no longer used for carousel)
void handleStorePurchasing(int8_t direction) {
    // This function is no longer used since carousel navigation
    // is handled by the nextHatInCarousel() function
    // Keeping for backwards compatibility
}

// Function to cycle to the next hat in the carousel
void nextHatInCarousel() {
    if (isPurchasing) {
        selectedHat++;
        if (selectedHat > 4) {
            selectedHat = 1; // Cycle back to tophat
        }
        Serial.print("Carousel moved to: ");
        Serial.print(HAT_NAMES[selectedHat]);
        Serial.print(" (");
        Serial.print(HAT_PRICES[selectedHat]);
        Serial.println(" stars)");
    }
}

// Function to toggle store purchasing mode
void toggleStorePurchasing() {
    isPurchasing = !isPurchasing;
    if (!isPurchasing) {
        Serial.println("Exiting store purchasing mode");
        // Reset cursor position to the up arrow (index 1)
        cursorPosition = 1;
    } else {
        Serial.println("Entering store purchasing mode");
        selectedHat = 1; // Reset to first hat option
        cursorPosition = 0; // Start with NEXT button selected
        // Reset logged message state when entering purchasing mode
        resetLoggedMessage();
    }
}

// Function to get current selected hat
uint8_t getSelectedHat() {
    return selectedHat;
}

// Function to check if player can afford a hat
bool canAffordHat(uint8_t hatType) {
    if (hatType == 0 || hatType > 4) return false;
    if (isHatPurchased(hatType)) return false; // Can't afford if already purchased
    return getStars() >= HAT_PRICES[hatType];
}

// Function to check if a hat is already purchased
bool isHatPurchased(uint8_t hatType) {
    if (hatType == 0 || hatType > 4) return false;
    return loadHatPurchased(hatType);
}

// Function to purchase a hat
bool purchaseHat(uint8_t hatType) {
    if (!canAffordHat(hatType)) {
        Serial.println("Not enough stars to purchase this hat!");
        return false;
    }
    
    // Check if hat is already purchased
    if (isHatPurchased(hatType)) {
        Serial.println("Hat is already purchased!");
        return false;
    }
    
    // Get external mutex and stars variable from main.cpp
    extern SemaphoreHandle_t petStateMutex;
    extern uint32_t stars;
    
    // Protect star update with mutex
    if (xSemaphoreTake(petStateMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        // Deduct stars
        uint32_t currentStars = stars;
        uint32_t newStars = currentStars - HAT_PRICES[hatType];
        
        // Update global stars variable
        stars = newStars;
        
        // Save new star count to persistent storage
        if (saveStars(stars)) {
            Serial.print("Purchased ");
            Serial.print(HAT_NAMES[hatType]);
            Serial.print(" for ");
            Serial.print(HAT_PRICES[hatType]);
            Serial.print(" stars! Remaining stars: ");
            Serial.println(stars);
            
            xSemaphoreGive(petStateMutex);
            
            // Save purchased hat to persistent storage
            if (saveHatPurchased(hatType, true)) {
                Serial.println("Hat purchase saved to storage!");
                
                // Auto-equip the newly purchased hat
                if (equipHat(hatType)) {
                    Serial.println("New hat auto-equipped!");
                } else {
                    Serial.println("Failed to auto-equip new hat");
                }
                
                return true;
            } else {
                Serial.println("Failed to save hat purchase to storage!");
                // Note: Stars were already saved, so we don't revert that
                return true; // Still consider purchase successful since stars were deducted
            }
        } else {
            Serial.println("Failed to save star count after purchase");
            // Revert stars change since save failed
            stars = currentStars;
            xSemaphoreGive(petStateMutex);
            return false;
        }
    } else {
        Serial.println("Failed to acquire mutex for star purchase");
        return false;
    }
}

// Function to check if a hat is currently equipped
bool isHatEquipped(uint8_t hatType) {
    if (hatType == 0 || hatType > 4) return false;
    return loadHatWearing(hatType);
}

// Function to unequip all hats (only one hat can be equipped at a time)
void unequipAllHats() {
    for (uint8_t i = 1; i <= 4; i++) {
        if (isHatEquipped(i)) {
            saveHatWearing(i, false);
            Serial.print("Unequipped ");
            Serial.println(HAT_NAMES[i]);
        }
    }
}

// Function to equip a hat (and unequip any currently equipped hat)
bool equipHat(uint8_t hatType) {
    if (hatType == 0 || hatType > 4) {
        Serial.println("Invalid hat type for equipping");
        return false;
    }
    
    // Check if hat is purchased
    if (!isHatPurchased(hatType)) {
        Serial.println("Cannot equip hat - not purchased");
        return false;
    }
    
    // Unequip all other hats first
    unequipAllHats();
    
    // Equip the selected hat
    if (saveHatWearing(hatType, true)) {
        Serial.print("Equipped ");
        Serial.println(HAT_NAMES[hatType]);
        return true;
    } else {
        Serial.println("Failed to save hat equip status");
        return false;
    }
}

// Function to get the currently equipped hat (returns 0 if no hat equipped)
uint8_t getCurrentlyEquippedHat() {
    for (uint8_t i = 1; i <= 4; i++) {
        if (isHatEquipped(i)) {
            return i;
        }
    }
    return 0; // No hat equipped
} 