#pragma once

#include <Arduino.h>
#include "screens.h"

// Structure to represent a selectable UI element
struct SelectableElement {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    const char* label;  // Optional label for debugging
};

// Maximum number of selectable elements per page
constexpr uint8_t MAX_SELECTABLE_ELEMENTS = 10;

// Structure to hold all selectable elements for a page
struct PageElements {
    SelectableElement elements[MAX_SELECTABLE_ELEMENTS];
    uint8_t count;
};

// Global cursor position (index into the current page's elements array)
extern uint8_t cursorPosition;

// Function declarations
void initializeCursor();
void moveCursor(int8_t direction);  // direction: -1 for previous, 1 for next
void handleCursorSelection();
void updateCursorDisplay(); 