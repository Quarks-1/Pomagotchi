#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include "screens.h"
#include "pet_state.h"
#include "cursor.h"
#include "water_logging.h"

// Pin definitions for E-Ink display
#define EPD_RST_PIN     16  // RST -> GPIO16
#define EPD_DC_PIN      17  // DC -> GPIO17
#define EPD_CS_PIN      5   // CS -> GPIO5
#define EPD_BUSY_PIN    4   // BUSY -> GPIO4
// MOSI -> GPIO23 (default SPI)
// SCK -> GPIO18 (default SPI)

// Pin definitions for rotary encoder
#define ENCODER_A_PIN   12  // CLK -> GPIO12
#define ENCODER_B_PIN   13  // DT -> GPIO13
#define ENCODER_BTN_PIN 14  // SW -> GPIO14

// Display dimensions
#define EPD_WIDTH       200
#define EPD_HEIGHT      200

// Global variables
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN));
Page currentPage = HOME_PAGE;
Page previousPage = HOME_PAGE;  // Track previous page for detecting changes

// Rotary encoder state
volatile int8_t encoderDelta = 0;
volatile bool encoderButtonPressed = false;
volatile unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_DELAY = 50;  // 50ms debounce time

// Function declarations
void initializeDisplay();
void handleInput();
void updateDisplay();
void changePage(Page newPage);
void handleEncoder();
void handleEncoderButton();

// Interrupt service routines for rotary encoder
void IRAM_ATTR encoderISR() {
    static uint8_t oldState = 0;
    uint8_t newState = (digitalRead(ENCODER_A_PIN) << 1) | digitalRead(ENCODER_B_PIN);
    uint8_t state = (oldState << 2) | newState;
    
    if (state == 0b1101 || state == 0b0100 || state == 0b0010 || state == 0b1011) {
        encoderDelta++;
    } else if (state == 0b1110 || state == 0b0111 || state == 0b0001 || state == 0b1000) {
        encoderDelta--;
    }
    
    oldState = newState;
}

void IRAM_ATTR buttonISR() {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > DEBOUNCE_DELAY) {
        encoderButtonPressed = true;
        lastButtonPress = currentTime;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Pomagotchi Starting up...");

    // Initialize SPI
    SPI.begin();
    
    // Initialize display
    initializeDisplay();
    
    // Initialize rotary encoder pins
    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);
    pinMode(ENCODER_BTN_PIN, INPUT_PULLUP);
    
    // Attach interrupts for rotary encoder
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), encoderISR, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_BTN_PIN), buttonISR, FALLING);
    
    // Initialize cursor
    initializeCursor();
    
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
    handleEncoder();
    handleEncoderButton();
    updatePetState();  // Update pet's hunger and thirst
    updateDisplay();
    delay(100); // Small delay to prevent too frequent updates
}

void handleEncoder() {
    if (encoderDelta != 0) {
        moveCursor(encoderDelta);
        encoderDelta = 0;
    }
}

void handleEncoderButton() {
    if (encoderButtonPressed) {
        handleCursorSelection();
        encoderButtonPressed = false;
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
        
        // Determine navigation direction
        bool isRightNavigation = false;
        switch (currentPage) {
            case HOME_PAGE:
                isRightNavigation = (newPage == DRINK_WATER_PAGE);
                break;
            case DRINK_WATER_PAGE:
                isRightNavigation = (newPage == FEEDING_PAGE);
                break;
            case FEEDING_PAGE:
                isRightNavigation = (newPage == STORE_PAGE);
                break;
            case STORE_PAGE:
                isRightNavigation = (newPage == HOME_PAGE);
                break;
        }

        previousPage = currentPage;
        currentPage = newPage;
        
        // Set cursor position based on navigation direction
        cursorPosition = isRightNavigation ? 0 : 1;
        
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
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();  // Remove whitespace
        
        // Print received command for debugging
        Serial.print("Received command: ");
        Serial.println(command);
        
        if (command == "left") {
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
        case STORE_PAGE:
            drawStorePage(display);
            break;
    }
} 