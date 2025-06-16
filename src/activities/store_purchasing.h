#pragma once

#include <Arduino.h>

// Hat definitions
enum HatType {
    NO_HAT = 0,
    TOPHAT = 1,
    COWBOY_HAT = 2,
    PARTY_HAT = 3,
    STAR_HAT = 4
};

// Hat prices in stars
constexpr uint8_t HAT_PRICES[] = {0, 10, 25, 50, 100}; // Index matches HatType enum
constexpr const char* HAT_NAMES[] = {"None", "Tophat", "Cowboy", "Party", "Star"};

// Store purchasing state
extern bool isPurchasing;
extern uint8_t selectedHat;

// Function declarations
void handleStorePurchasing(int8_t direction);
void toggleStorePurchasing();
uint8_t getSelectedHat();
bool canAffordHat(uint8_t hatType);
bool purchaseHat(uint8_t hatType);
bool isHatPurchased(uint8_t hatType);
void nextHatInCarousel();

// Hat equipping functions
bool isHatEquipped(uint8_t hatType);
bool equipHat(uint8_t hatType);
void unequipAllHats();
uint8_t getCurrentlyEquippedHat(); 