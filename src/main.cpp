#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include "screens.h"
#include "pet_state.h"

// Pin definitions for E-Ink display
#define EPD_RST_PIN     16  // RST -> GPIO16
#define EPD_DC_PIN      17  // DC -> GPIO17
#define EPD_CS_PIN      5   // CS -> GPIO5
#define EPD_BUSY_PIN    4   // BUSY -> GPIO4
// MOSI -> GPIO23 (default SPI)
// SCK -> GPIO18 (default SPI)

// Display dimensions
#define EPD_WIDTH       200
#define EPD_HEIGHT      200

// Global variables
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN));
Page currentPage = HOME_PAGE;
Page previousPage = HOME_PAGE;  // Track previous page for detecting changes

// Function declarations
void initializeDisplay();
void handleInput();
void updateDisplay();
void changePage(Page newPage);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Pomagotchi Starting up...");

    // Initialize SPI
    SPI.begin();
    
    // Initialize display
    initializeDisplay();
    
    // Set initial page to home
    currentPage = DRINK_WATER_PAGE;
    previousPage = DRINK_WATER_PAGE;
    Serial.println("Starting at Water Page");
    
    // Draw initial page
    drawDrinkWaterPage(display);
    Serial.println("Water Page drawn successfully");
}

void loop() {
    handleInput();
    updatePetState();  // Update pet's hunger and thirst
    updateDisplay();
    delay(100); // Small delay to prevent too frequent updates
}

void initializeDisplay() {
    display.init(115200);
    display.setRotation(1);
    display.clearScreen();
    display.fillScreen(GxEPD_WHITE);  // Set background to white
    display.display();
}

void changePage(Page newPage) {
    if (newPage != currentPage) {
        previousPage = currentPage;
        currentPage = newPage;
        
        // Full refresh when changing pages
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        
        // Draw the new page
        switch (currentPage) {
            case HOME_PAGE:
                drawHomePage(display);
                break;
            case DRINK_WATER_PAGE:
                drawDrinkWaterPage(display);
                break;
            case FEEDING_PAGE:
                drawFeedingPage(display);
                break;
            case STORE_PAGE:
                drawStorePage(display);
                break;
        }
        
        display.display();
    }
}

void handleInput() {
    // TODO: Implement input handling for navigation between pages
    // This will be implemented based on your input method (buttons, touch, etc.)
}

void updateDisplay() {
    // Update the current page with new values
    switch (currentPage) {
        case HOME_PAGE:
            drawHomePage(display);
            break;
        case DRINK_WATER_PAGE:
            drawDrinkWaterPage(display);
            break;
        case FEEDING_PAGE:
            drawFeedingPage(display);
            break;
        case STORE_PAGE:
            drawStorePage(display);
            break;
    }
} 