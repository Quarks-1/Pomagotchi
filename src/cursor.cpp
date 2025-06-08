#include "cursor.h"
#include "screens.h"
#include "ui_components.h"
#include "navigation.h"
#include "pet_state.h"
#include "water_logging.h"

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
        {115, 170, 85, 30, "Go to feeding"}
    },
    .count = 3
};

static PageElements feedingPageElements = {
    .elements = {
        // Left navigation button
        {10, 170, 85, 30, "Go to water page"},
        // Up navigation button
        {90, 170, 30, 30, "Enter feeding"},
        // Right navigation button
        {115, 170, 85, 30, "Go to pet pommy"}
    },
    .count = 3
};

static PageElements petPommyPageElements = {
    .elements = {
        // Left navigation button
        {10, 170, 85, 30, "Go to feeding"},
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
        case FEEDING_PAGE:
            return &feedingPageElements;
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
                    // Update thirst level if exiting water logging mode
                    if (!isWaterLogging) {
                        thirst = constrain(thirst + getWaterFillLevel(), 0, 100);
                    }
                } else if (cursorPosition == 2) {
                    changePage(FEEDING_PAGE);
                }
                break;
                
            case FEEDING_PAGE:
                if (cursorPosition == 0) {
                    changePage(DRINK_WATER_PAGE);
                } else if (cursorPosition == 1) {
                    // TODO: Implement feeding action
                } else if (cursorPosition == 2) {
                    changePage(PET_POMMY_PAGE);
                }
                break;

            case PET_POMMY_PAGE:
                if (cursorPosition == 0) {
                    changePage(FEEDING_PAGE);
                } else if (cursorPosition == 1) {
                    // TODO: Implement pet pommy action
                } else if (cursorPosition == 2) {
                    changePage(STORE_PAGE);
                }
                break;
                
            case STORE_PAGE:
                if (cursorPosition == 0) {
                    changePage(PET_POMMY_PAGE);
                } else if (cursorPosition == 1) {
                    // TODO: Implement store action
                } else if (cursorPosition == 2) {
                    changePage(HOME_PAGE);
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