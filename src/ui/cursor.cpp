#include "cursor.h"
#include "screens/screens.h"
#include "ui_components.h"
#include "navigation.h"
#include "../pet/pet_state.h"
#include "../activities/water_logging.h"
#include "../activities/sunbathing.h"
#include "../pet/pet_pommy.h"
#include "../activities/store_purchasing.h"
#include "logged_message.h"

// Global cursor position
uint8_t cursorPosition = 0;

// Arrays to hold selectable elements for each page
static PageElements homePageElements = {
    .elements = {
        // Left navigation button
        {10, 170, 90, 30, "Go to store"},
        // Right navigation button
        {110, 170, 90, 30, "Go to water logging"}
    },
    .count = 2
};

static PageElements waterPageElements = {
    .elements = {
        // Left navigation button
        {10, 170, 85, 30, "Go to home page"},
        // Up navigation button
        {90, 170, 30, 30, "Enter water logging"},
        // Right navigation button
        {115, 170, 85, 30, "Go to sunbathing"}
    },
    .count = 3
};

static PageElements sunbathingPageElements = {
    .elements = {
        // Left navigation button
        {10, 170, 85, 30, "Go to water page"},
        // Up navigation button
        {90, 170, 30, 30, "Enter sunbathing"},
        // Right navigation button
        {115, 170, 85, 30, "Go to pet pommy"}
    },
    .count = 3
};

static PageElements petPommyPageElements = {
    .elements = {
        // Left navigation button
        {10, 170, 85, 30, "Go to sunbathing"},
        // Up navigation button
        {90, 170, 30, 30, "Pet pommy"},
        // Right navigation button
        {115, 170, 85, 30, "Go to store"}
    },
    .count = 3
};

static PageElements storePageElements = {
    .elements = {
        // Left navigation button
        {10, 170, 85, 30, "Go to pet pommy"},
        // Up navigation button
        {90, 170, 30, 30, "Enter store"},
        // Right navigation button
        {115, 170, 85, 30, "Go to home page"}
    },
    .count = 3
};

// Get the current page's elements
static PageElements* getCurrentPageElements() {
    switch (currentPage) {
        case HOME_PAGE:
            return &homePageElements;
        case DRINK_WATER_PAGE:
            return &waterPageElements;
        case SUNBATHE_PAGE:
            return &sunbathingPageElements;
        case PET_POMMY_PAGE:
            return &petPommyPageElements;
        case STORE_PAGE:
            return &storePageElements;
        default:
            return &homePageElements;
    }
}

void initializeCursor() {
    cursorPosition = 0;
}

void moveCursor(int8_t direction) {
    PageElements* currentElements = getCurrentPageElements();
    
    // If in water logging mode, handle water level adjustment
    if (currentPage == DRINK_WATER_PAGE && isWaterLogging) {
        handleWaterLogging(direction);
        return;
    }
    
    // If in sunbathing mode, handle sunlight level adjustment
    if (currentPage == SUNBATHE_PAGE && isSunbathing) {
        handleSunbathing(direction);
        return;
    }
    
    // If in petting mode, handle pet level adjustment
    if (currentPage == PET_POMMY_PAGE && isPetting) {
        handlePetting(direction);
        return;
    }
    
    // If in store purchasing mode, handle hat selection
    if (currentPage == STORE_PAGE && isPurchasing) {
        // In carousel mode, just handle normal cursor movement between buttons
        // Hat cycling is now handled by the NEXT button, not cursor movement
        int16_t newPosition = cursorPosition + direction;
        if (newPosition >= 0 && newPosition < 3) { // 3 buttons: NEXT, BUY, EXIT
            cursorPosition = newPosition;
            updateCursorDisplay();
        }
        return;
    }
    
    // Calculate new position without wrapping
    int16_t newPosition = cursorPosition + direction;
    if (newPosition >= 0 && newPosition < currentElements->count) {
        cursorPosition = newPosition;
        // Update display to show new cursor position
        updateCursorDisplay();
    }
}

void handleCursorSelection() {
    PageElements* currentElements = getCurrentPageElements();
    if (cursorPosition < currentElements->count) {
        SelectableElement& element = currentElements->elements[cursorPosition];
        
        // Handle selection based on current page and cursor position
        switch (currentPage) {
            case HOME_PAGE:
                if (cursorPosition == 0) {
                    changePage(STORE_PAGE);
                } else if (cursorPosition == 1) {
                    changePage(DRINK_WATER_PAGE);
                }
                break;
                
            case DRINK_WATER_PAGE:
                if (cursorPosition == 0) {
                    changePage(HOME_PAGE);
                } else if (cursorPosition == 1) {
                    // Toggle water logging mode
                    toggleWaterLogging();
                } else if (cursorPosition == 2) {
                    changePage(SUNBATHE_PAGE);
                }
                break;
                
            case SUNBATHE_PAGE:
                if (cursorPosition == 0) {
                    changePage(DRINK_WATER_PAGE);
                } else if (cursorPosition == 1) {
                    // Toggle sunbathing mode
                    toggleSunbathing();
                } else if (cursorPosition == 2) {
                    changePage(PET_POMMY_PAGE);
                }
                break;

            case PET_POMMY_PAGE:
                if (cursorPosition == 0) {
                    changePage(SUNBATHE_PAGE);
                } else if (cursorPosition == 1) {
                    // Toggle petting mode
                    togglePetting();
                } else if (cursorPosition == 2) {
                    changePage(STORE_PAGE);
                }
                break;
                
            case STORE_PAGE:
                if (isPurchasing) {
                    // Carousel mode: NEXT, BUY, EXIT buttons
                    if (cursorPosition == 0) {
                        // NEXT button - cycle to next hat
                        nextHatInCarousel();
                    } else if (cursorPosition == 1) {
                        // BUY/EQUIP button - purchase or equip current hat
                        uint8_t currentHat = getSelectedHat();
                        
                        if (isHatPurchased(currentHat)) {
                            // Hat is owned - equip it
                            Serial.print("Attempting to equip ");
                            Serial.println(HAT_NAMES[currentHat]);
                            
                            if (equipHat(currentHat)) {
                                Serial.println("Equip successful!");
                                showLoggedMessageNow();
                            } else {
                                Serial.println("Equip failed");
                            }
                        } else {
                            // Hat is not owned - purchase it
                            Serial.print("Attempting to purchase ");
                            Serial.println(HAT_NAMES[currentHat]);
                            
                            if (purchaseHat(currentHat)) {
                                Serial.println("Purchase successful!");
                                showLoggedMessageNow();
                            } else {
                                Serial.println("Purchase failed");
                            }
                        }
                    } else if (cursorPosition == 2) {
                        // EXIT button - exit purchasing mode
                        toggleStorePurchasing();
                    }
                } else {
                    // Regular navigation mode
                    if (cursorPosition == 0) {
                        changePage(PET_POMMY_PAGE);
                    } else if (cursorPosition == 1) {
                        // Toggle store purchasing mode
                        toggleStorePurchasing();
                    } else if (cursorPosition == 2) {
                        changePage(HOME_PAGE);
                    }
                }
                break;
        }
    }
}

void updateCursorDisplay() {
    PageElements* currentElements = getCurrentPageElements();
    if (cursorPosition < currentElements->count) {
        SelectableElement& element = currentElements->elements[cursorPosition];
        // Update the display to show the selected element
        // This will be handled by the individual page drawing functions
        // which will check cursorPosition to determine which element is selected
    }
} 