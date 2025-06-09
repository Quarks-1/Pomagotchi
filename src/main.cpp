#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include "screens.h"
#include "pet_state.h"
#include "cursor.h"
#include "water_logging.h"
#include "main.h"
#include "encoder.h"
#include "persistent_storage.h"

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
bool isDebugMode = true;  // Global debug mode flag
bool isEncoderEnabled = false;  // Global flag to enable/disable encoder functionality

// Create encoder instance
Encoder encoder;

// Function declarations
void initializeDisplay();
void handleInput();
void updateDisplay();
void changePage(Page newPage);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Pomagotchi Starting up...");

    // Initialize persistent storage
    initializeStorage();
    
    // Initialize pet state (loads saved values)
    initializePetState();

    // Initialize SPI
    SPI.begin();
    
    // Initialize display
    initializeDisplay();
    
    // Initialize encoder
    encoder.begin();
    
    // Initialize cursor
    initializeCursor();
    
    // Set initial page to home
    currentPage = HOME_PAGE;
    previousPage = HOME_PAGE;
    Serial.println("Starting at Home Page");
    
    // Draw initial page
    drawHomePage(display);
    Serial.println("Home Page drawn successfully");
}

void loop() {
    handleInput();
    
    // Only process encoder if enabled
    if (encoder.isEnabled()) {
        // Update encoder state - no delay for responsiveness
        encoder.update();
        
        // Handle encoder rotation
        int8_t delta = encoder.getDelta();
        if (delta != 0) {
            moveCursor(delta);
        }
        
        // Handle encoder button
        if (encoder.getButtonPressed()) {
            handleCursorSelection();
            encoder.resetButtonState();
        }
    }
    
    static unsigned long lastScreenUpdate = 0;
    unsigned long currentMillis = millis();
    
    // Only update screen every 100ms
    if (currentMillis - lastScreenUpdate >= 100) {
        updatePetState();  // Update pet's hunger and thirst
        updateDisplay();
        lastScreenUpdate = currentMillis;
    }
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
        // Reset logged message state when changing pages
        resetLoggedMessage();
        
        // Set cursor position based on the navigation flow
        switch (currentPage) {
            case HOME_PAGE:
                cursorPosition = (newPage == STORE_PAGE) ? 2 : 0;
                break;
            case DRINK_WATER_PAGE:
                cursorPosition = (newPage == HOME_PAGE) ? 1 : 0;
                break;
            case FEEDING_PAGE:
                cursorPosition = (newPage == DRINK_WATER_PAGE) ? 2 : 0;
                break;
            case PET_POMMY_PAGE:
                cursorPosition = (newPage == FEEDING_PAGE) ? 2 : 0;
                break;
            case STORE_PAGE:
                cursorPosition = (newPage == PET_POMMY_PAGE) ? 2 : 0;
                break;
        }

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
            case PET_POMMY_PAGE:
                drawPetPommyPage(display);
                break;
            case STORE_PAGE:
                drawStorePage(display);
                break;
        }
        
        display.display();
    }
}

void handleInput() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();  // Remove whitespace
        
        // Print received command for debugging
        Serial.print("Received command: ");
        Serial.println(command);
        
        if (command == "debug") {
            isDebugMode = !isDebugMode;
            Serial.print("Debug mode: ");
            Serial.println(isDebugMode ? "ON" : "OFF");
        }
        else if (command == "left") {
            moveCursor(-1);
            Serial.println("Moved cursor left");
        }
        else if (command == "right") {
            moveCursor(1);
            Serial.println("Moved cursor right");
        }
        else if (command == "enter") {
            handleCursorSelection();
            Serial.println("Selected current item");
        }
        else if (command == "home") {
            changePage(HOME_PAGE);
            Serial.println("Changed to home page");
        }
        else if (command == "water") {
            changePage(DRINK_WATER_PAGE);
            Serial.println("Changed to water page");
        }
        else if (command == "feed") {
            changePage(FEEDING_PAGE);
            Serial.println("Changed to feeding page");
        }
        else if (command == "pet") {
            changePage(PET_POMMY_PAGE);
            Serial.println("Changed to pet pommy page");
        }
        else if (command == "store") {
            changePage(STORE_PAGE);
            Serial.println("Changed to store page");
        }
        else if (command.startsWith("set_thirst ")) {
            // Extract number after "set_thirst "
            int value = command.substring(11).toInt();
            thirst = constrain(value, 0, 100);
            Serial.print("Set thirst to: ");
            Serial.println(thirst);
        }
        else if (command.startsWith("set_sunlight ")) {
            // Extract number after "set_sunlight "
            int value = command.substring(13).toInt();
            sunlight = constrain(value, 0, 100);
            Serial.print("Set sunlight to: ");
            Serial.println(sunlight);
        }
        else if (command == "status") {
            Serial.println("Current Status:");
            Serial.print("Thirst: ");
            Serial.println(thirst);
            Serial.print("Sunlight: ");
            Serial.println(sunlight);
            Serial.print("Current Page: ");
            switch (currentPage) {
                case HOME_PAGE:
                    Serial.println("Home");
                    break;
                case DRINK_WATER_PAGE:
                    Serial.println("Water");
                    break;
                case FEEDING_PAGE:
                    Serial.println("Feeding");
                    break;
                case STORE_PAGE:
                    Serial.println("Store");
                    break;
            }
        }
        else if (command == "help") {
            Serial.println("Available commands:");
            Serial.println("left - Move cursor left");
            Serial.println("right - Move cursor right");
            Serial.println("enter - Select current item");
            Serial.println("home - Go to home page");
            Serial.println("water - Go to water page");
            Serial.println("feed - Go to feeding page");
            Serial.println("store - Go to store page");
            Serial.println("set_thirst <0-100> - Set thirst level");
            Serial.println("set_sunlight <0-100> - Set sunlight level");
            Serial.println("status - Show current status");
            Serial.println("help - Show this help message");
        }
        else {
            Serial.println("Unknown command. Type 'help' for available commands.");
        }
    }
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
        case PET_POMMY_PAGE:
            drawPetPommyPage(display);
            break;
        case STORE_PAGE:
            drawStorePage(display);
            break;
    }
}

// Function to check if debug mode is active
bool isDebugModeActive() {
    return isDebugMode;
} 