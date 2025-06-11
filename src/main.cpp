#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include "logged_message.h"
#include "screens.h"
#include "pet_state.h"
#include "cursor.h"
#include "water_logging.h"
#include "main.h"
#include "encoder.h"
#include "persistent_storage.h"
#include "light_sensor.h"
#include "sunbathing.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

// Pin definitions for E-Ink display
#define EPD_RST_PIN     13  // RST -> GPIO13
#define EPD_DC_PIN      12  // DC -> GPIO12
#define EPD_CS_PIN      14  // CS -> GPIO14
#define EPD_BUSY_PIN    15  // BUSY -> GPIO15
// MOSI -> GPIO35 (default SPI)
// SCK -> GPIO36 (default SPI)

// Display dimensions
#define EPD_WIDTH       200
#define EPD_HEIGHT      200

// Global variables
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(EPD_CS_PIN, EPD_DC_PIN, EPD_RST_PIN, EPD_BUSY_PIN));
Page currentPage = HOME_PAGE;
Page previousPage = HOME_PAGE;  // Track previous page for detecting changes
bool isDebugMode = true;  // Global debug mode flag
bool isEncoderEnabled = true;  // Global flag to enable/disable encoder functionality
bool isLightSensorEnabled = true; // Global flag to enable/disable light sensor functionality

// Create encoder instance
Encoder encoder;

// FreeRTOS task handles
TaskHandle_t inputTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;

// Queue for input events
QueueHandle_t inputQueue = NULL;

// Input event structure
struct InputEvent {
    enum Type {
        ENCODER_DELTA,
        ENCODER_BUTTON,
        SERIAL_INPUT
    } type;
    int16_t value;  // Changed from int8_t to int16_t to accommodate Serial.read()
};

// Function declarations
void initializeDisplay();
void handleInput();
void updateDisplay();
void changePage(Page newPage);

// Input handling task
void inputTask(void *parameter) {
    static unsigned long lastSensorRead = 0;
    const unsigned long SENSOR_READ_INTERVAL = 1000; // Read sensor every second
    
    while (1) {
        // Update encoder state
        encoder.update();
        
        // Handle encoder rotation
        int8_t delta = encoder.getDelta();
        if (delta != 0) {
            InputEvent event = {InputEvent::ENCODER_DELTA, static_cast<int16_t>(delta)};
            xQueueSend(inputQueue, &event, 0);
        }
        
        // Handle encoder button
        if (encoder.getButtonPressed()) {
            InputEvent event = {InputEvent::ENCODER_BUTTON, 0};
            xQueueSend(inputQueue, &event, 0);
            encoder.resetButtonState();
        }
        
        // Handle serial input
        if (Serial.available()) {
            InputEvent event = {InputEvent::SERIAL_INPUT, Serial.read()};
            xQueueSend(inputQueue, &event, 0);
        }
        
        // Read and print light sensor value periodically
        unsigned long currentTime = millis();
        if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
            if (isLightSensorEnabled) {
                uint16_t lightLevel = getLightLevel();
                Serial.print("Light sensor reading: ");
                Serial.print(lightLevel);
                Serial.println(" (raw value)");
                Serial.print("In sunlight: ");
                Serial.println(isInSunlight() ? "Yes" : "No");
            } else {
                Serial.println("Light sensor is disabled");
            }
            lastSensorRead = currentTime;
        }
        
        // Small delay to prevent task from hogging CPU
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Display update task
void displayTask(void *parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(100); // 100ms display update interval
    String commandBuffer = "";
    
    while (1) {
        // Process any pending input events
        InputEvent event;
        while (xQueueReceive(inputQueue, &event, 0) == pdTRUE) {
            switch (event.type) {
                case InputEvent::ENCODER_DELTA:
                    moveCursor(event.value > 0 ? 1 : -1);
                    break;
                case InputEvent::ENCODER_BUTTON:
                    handleCursorSelection();
                    break;
                case InputEvent::SERIAL_INPUT:
                    // Handle serial input
                    char c = event.value;
                    if (c == '\n' || c == '\r') {
                        if (commandBuffer.length() > 0) {
                            // Process the command
                            if (commandBuffer == "debug") {
                                isDebugMode = !isDebugMode;
                                Serial.print("Debug mode: ");
                                Serial.println(isDebugMode ? "ON" : "OFF");
                            }
                            else if (commandBuffer == "left") {
                                moveCursor(-1);
                                Serial.println("Moved cursor left");
                            }
                            else if (commandBuffer == "right") {
                                moveCursor(1);
                                Serial.println("Moved cursor right");
                            }
                            else if (commandBuffer == "enter") {
                                handleCursorSelection();
                                Serial.println("Selected current item");
                            }
                            else if (commandBuffer == "home") {
                                changePage(HOME_PAGE);
                                Serial.println("Changed to home page");
                            }
                            else if (commandBuffer == "water") {
                                changePage(DRINK_WATER_PAGE);
                                Serial.println("Changed to water page");
                            }
                            else if (commandBuffer == "sun") {
                                changePage(SUNBATHE_PAGE);
                                Serial.println("Changed to sunbathing page");
                            }
                            else if (commandBuffer == "pet") {
                                changePage(PET_POMMY_PAGE);
                                Serial.println("Changed to pet pommy page");
                            }
                            else if (commandBuffer == "store") {
                                changePage(STORE_PAGE);
                                Serial.println("Changed to store page");
                            }
                            else if (commandBuffer.startsWith("set_thirst ")) {
                                // Extract number after "set_thirst "
                                int value = commandBuffer.substring(11).toInt();
                                thirst = constrain(value, 0, 100);
                                Serial.print("Set thirst to: ");
                                Serial.println(thirst);
                            }
                            else if (commandBuffer.startsWith("set_sunlight ")) {
                                // Extract number after "set_sunlight "
                                int value = commandBuffer.substring(13).toInt();
                                sunlight = constrain(value, 0, 100);
                                Serial.print("Set sunlight to: ");
                                Serial.println(sunlight);
                            }
                            else if (commandBuffer == "status") {
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
                                    case SUNBATHE_PAGE:
                                        Serial.println("Sunbathing");
                                        break;
                                    case STORE_PAGE:
                                        Serial.println("Store");
                                        break;
                                }
                            }
                            else if (commandBuffer == "help") {
                                Serial.println("Available commands:");
                                Serial.println("left - Move cursor left");
                                Serial.println("right - Move cursor right");
                                Serial.println("enter - Select current item");
                                Serial.println("home - Go to home page");
                                Serial.println("water - Go to water page");
                                Serial.println("sun - Go to sunbathing page");
                                Serial.println("pet - Go to pet pommy page");
                                Serial.println("store - Go to store page");
                                Serial.println("set_thirst <0-100> - Set thirst level");
                                Serial.println("set_sunlight <0-100> - Set sunlight level");
                                Serial.println("status - Show current status");
                                Serial.println("help - Show this help message");
                                Serial.println("add - Increase sunlight level (only works in sunbathing mode)");
                            }
                            commandBuffer = "";
                        }
                    } else {
                        commandBuffer += c;
                    }
                    break;
            }
        }
        
        // Update pet state
        updatePetState();
        
        // Update display
        updateDisplay();
        
        // Wait for next update interval
        vTaskDelay(xDelay);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Pomagotchi...");
    
    // Initialize I2C for STEMMA QT connector
    Wire.begin();  // Use default I2C pins for Feather ESP32 V2 (SDA=23, SCL=22)
    
    // Initialize encoder
    encoder.begin();
    
    // Initialize storage
    if (!initializeStorage()) {
        Serial.println("Failed to initialize storage");
    }
    
    // Initialize light sensor
    if (!initializeLightSensor()) {
        Serial.println("Light sensor not found - continuing without it");
    }
    
    // Initialize pet state
    initializePetState();
    
    // Initialize display
    display.init(115200);
    display.setRotation(1);
    display.clearScreen();
    display.fillScreen(GxEPD_WHITE);
    
    // Create input queue
    inputQueue = xQueueCreate(10, sizeof(InputEvent));
    
    // Create tasks
    xTaskCreate(
        inputTask,      // Task function
        "InputTask",    // Task name
        4096,          // Stack size
        NULL,          // Task parameters
        2,             // Task priority
        &inputTaskHandle
    );
    
    xTaskCreate(
        displayTask,    // Task function
        "DisplayTask",  // Task name
        4096,          // Stack size
        NULL,          // Task parameters
        1,             // Task priority
        &displayTaskHandle
    );
    
    // Draw initial page
    currentPage = HOME_PAGE;
    drawHomePage(display);
}

void loop() {
    // Main loop is now empty as tasks handle everything
    vTaskDelay(pdMS_TO_TICKS(1000)); // Prevent watchdog timer from triggering
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
        // Reset logged message state when changing to/from water or sunbathing pages
        if (currentPage == DRINK_WATER_PAGE || newPage == DRINK_WATER_PAGE) {
            resetLoggedMessage();
        }
        if (currentPage == SUNBATHE_PAGE || newPage == SUNBATHE_PAGE) {
            resetLoggedMessage();
        }
        
        // Set cursor position based on the navigation flow
        switch (currentPage) {
            case HOME_PAGE:
                cursorPosition = (newPage == STORE_PAGE) ? 2 : 0;
                break;
            case DRINK_WATER_PAGE:
                cursorPosition = (newPage == HOME_PAGE) ? 1 : 0;
                break;
            case SUNBATHE_PAGE:
                cursorPosition = (newPage == DRINK_WATER_PAGE) ? 2 : 0;
                break;
            case PET_POMMY_PAGE:
                cursorPosition = (newPage == SUNBATHE_PAGE) ? 2 : 0;
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
            case SUNBATHE_PAGE:
                drawSunbathingPage(display);
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
        else if (command == "sun") {
            changePage(SUNBATHE_PAGE);
            Serial.println("Changed to sunbathing page");
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
                case SUNBATHE_PAGE:
                    Serial.println("Sunbathing");
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
            Serial.println("sun - Go to sunbathing page");
            Serial.println("pet - Go to pet pommy page");
            Serial.println("store - Go to store page");
            Serial.println("set_thirst <0-100> - Set thirst level");
            Serial.println("set_sunlight <0-100> - Set sunlight level");
            Serial.println("status - Show current status");
            Serial.println("help - Show this help message");
            Serial.println("add - Increase sunlight level (only works in sunbathing mode)");
        }
        else if (command == "add") {
            if (currentPage == SUNBATHE_PAGE && isSunbathing) {
                if (sunlightFillLevel < 100) {
                    sunlightFillLevel++;
                    Serial.print("Manually increased sunlight fill level to: ");
                    Serial.println(sunlightFillLevel);
                } else {
                    Serial.println("Sunlight fill level already at maximum (100)");
                }
            } else {
                Serial.println("Can only add sunlight while in sunbathing mode");
            }
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
        case SUNBATHE_PAGE:
            drawSunbathingPage(display);
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