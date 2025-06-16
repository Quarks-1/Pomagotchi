#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include "ui/logged_message.h"
#include "ui/screens/screens.h"
#include "pet/pet_state.h"
#include "ui/cursor.h"
#include "activities/water_logging.h"
#include "main.h"
#include "hardware/encoder.h"
#include "storage/persistent_storage.h"
#include "hardware/light_sensor.h"
#include "hardware/battery_monitor.h"
#include "activities/sunbathing.h"
#include "pet/pet_pommy.h"
#include "activities/store_purchasing.h"
#include "tasks.h"
#include "system/sleep_manager.h"
#include "system/sleep_message.h"

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
bool isDebugMode = false;  // Global debug mode flag
bool isEncoderEnabled = true;  // Global flag to enable/disable encoder functionality
bool isLightSensorEnabled = true; // Global flag to enable/disable light sensor functionality

// Create encoder instance
Encoder encoder;

// Function declarations
void initializeDisplay();
void handleInput();
void updateDisplay();
void changePage(Page newPage);
void handleSerialInput(char c);

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Pomagotchi...");
    
    // Initialize I2C for STEMMA QT connector
    Wire.begin();  // Use default I2C pins for Feather ESP32 V2 (SDA=23, SCL=22)
    
    // Initialize encoder
    encoder.begin();
    
    // Initialize storage
    Serial.println("Initializing storage...");
    if (!initializeStorage()) {
        Serial.println("Failed to initialize storage");
    } else {
        Serial.println("Storage initialized successfully");
    }
    
    // Initialize light sensor
    if (!initializeLightSensor()) {
        Serial.println("Light sensor not found - continuing without it");
    }
    
    // Initialize battery monitor
    if (!initializeBatteryMonitor()) {
        Serial.println("Battery monitor not found - continuing without it");
    }
    
    // Initialize pet state
    Serial.println("Initializing pet state...");
    initializePetState();
    Serial.print("Initial sunlight value: ");
    Serial.println(sunlight);
    
    // Initialize sleep manager
    Serial.println("Initializing sleep manager...");
    initializeSleepManager();
    
    // Initialize display
    display.init(115200);
    display.setRotation(1);
    display.clearScreen();
    display.fillScreen(GxEPD_WHITE);
    
    // Set initial page (displayTask will handle the drawing)
    currentPage = HOME_PAGE;
    
    // Create and start tasks
    createTasks();
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
        // Reset logged message state when changing to/from activity pages
        if (currentPage == DRINK_WATER_PAGE || newPage == DRINK_WATER_PAGE) {
            resetLoggedMessage();
        }
        if (currentPage == SUNBATHE_PAGE || newPage == SUNBATHE_PAGE) {
            resetLoggedMessage();
        }
        if (currentPage == PET_POMMY_PAGE || newPage == PET_POMMY_PAGE) {
            resetLoggedMessage();
        }
        
        // Reset sleep message when changing away from home page
        if (currentPage == HOME_PAGE || newPage == HOME_PAGE) {
            resetSleepMessage();
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
        
        // Protect display operations with mutex
        if (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
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
            xSemaphoreGive(displayMutex);
        }
    }
}

void handleSerialInput(char c) {
    static String commandBuffer = "";
    
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
            else if (commandBuffer == "stars") {
                Serial.print("Total stars earned: ");
                Serial.println(getStars());
            }
            else if (commandBuffer.startsWith("set_stars ")) {
                // Extract number after "set_stars "
                uint32_t value = commandBuffer.substring(10).toInt();
                if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
                    stars = value;
                    saveStars(stars);
                    xSemaphoreGive(petStateMutex);
                }
                Serial.print("Set stars to: ");
                Serial.println(stars);
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
                if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
                    thirst = constrain(value, 0, 100);
                    StorageEvent event = {StorageEvent::SAVE_THIRST, thirst};
                    xQueueSend(storageQueue, &event, 0);
                    xSemaphoreGive(petStateMutex);
                }
                Serial.print("Set thirst to: ");
                Serial.println(thirst);
            }
            else if (commandBuffer.startsWith("set_sunlight ")) {
                // Extract number after "set_sunlight "
                int value = commandBuffer.substring(13).toInt();
                if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
                    sunlight = constrain(value, 0, 100);
                    StorageEvent event = {StorageEvent::SAVE_SUNLIGHT, sunlight};
                    xQueueSend(storageQueue, &event, 0);
                    xSemaphoreGive(petStateMutex);
                }
                Serial.print("Set sunlight to: ");
                Serial.println(sunlight);
            }
            else if (commandBuffer.startsWith("set_pets ")) {
                // Extract number after "set_pets "
                int value = commandBuffer.substring(9).toInt();
                if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
                    petStatus = constrain(value, 0, 10);
                    StorageEvent event = {StorageEvent::SAVE_PET_STATUS, petStatus};
                    xQueueSend(storageQueue, &event, 0);
                    xSemaphoreGive(petStateMutex);
                }
                Serial.print("Set pet status to: ");
                Serial.println(petStatus);
            }
            else if (commandBuffer.startsWith("set_proximity ")) {
                // Extract number after "set_proximity "
                int value = commandBuffer.substring(14).toInt();
                setProximityThreshold(value);
            }
            else if (commandBuffer == "proximity") {
                Serial.print("Current proximity value: ");
                Serial.println(getProximityLevel());
                Serial.print("Current proximity threshold: ");
                Serial.println(getProximityThreshold());
            }
            else if (commandBuffer == "status") {
                Serial.println("Current Status:");
                if (xSemaphoreTake(petStateMutex, portMAX_DELAY) == pdTRUE) {
                    Serial.print("Thirst: ");
                    Serial.println(thirst);
                    Serial.print("Sunlight: ");
                    Serial.println(sunlight);
                    Serial.print("Pet Status: ");
                    Serial.println(petStatus);
                    xSemaphoreGive(petStateMutex);
                }
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
                    case PET_POMMY_PAGE:
                        Serial.println("Pet Pommy");
                        break;
                    case STORE_PAGE:
                        Serial.println("Store");
                        break;
                }
            }
            else if (commandBuffer == "sleep_enable") {
                sleepManager.setSleepEnabled(true);
                Serial.println("Sleep mode enabled");
            }
            else if (commandBuffer == "sleep_disable") {
                sleepManager.setSleepEnabled(false);
                Serial.println("Sleep mode disabled");
            }
            else if (commandBuffer == "sleep_status") {
                Serial.print("Sleep enabled: ");
                Serial.println(sleepManager.isInLightSleep() ? "NO (currently in sleep)" : "YES");
                Serial.print("Time since last activity: ");
                Serial.print(sleepManager.getTimeSinceLastActivity() / 1000);
                Serial.println(" seconds");
                Serial.print("Sleep timeout: ");
                Serial.print(INACTIVITY_TIMEOUT_MS / 1000);
                Serial.println(" seconds");
            }
            else if (commandBuffer == "sleep_now") {
                Serial.println("Forcing light sleep...");
                sleepManager.enterLightSleep();
            }
            else if (commandBuffer == "sleep_test") {
                Serial.println("Testing sleep message display and entering sleep...");
                sleepManager.enterLightSleep();
            }
            else if (commandBuffer == "stack_info") {
                Serial.println("Task Stack Information:");
                Serial.print("Input task free stack: ");
                Serial.println(uxTaskGetStackHighWaterMark(inputTaskHandle));
                Serial.print("Display task free stack: ");
                Serial.println(uxTaskGetStackHighWaterMark(displayTaskHandle));
                Serial.print("Storage task free stack: ");
                Serial.println(uxTaskGetStackHighWaterMark(storageTaskHandle));
                Serial.print("Logic task free stack: ");
                Serial.println(uxTaskGetStackHighWaterMark(logicTaskHandle));
                Serial.print("Sleep task free stack: ");
                Serial.println(uxTaskGetStackHighWaterMark(sleepTaskHandle));
                Serial.print("Free heap: ");
                Serial.println(esp_get_free_heap_size());
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
                Serial.println("set_pets <0-10> - Set pet status level");
                Serial.println("set_proximity <value> - Set proximity threshold");
                Serial.println("proximity - Show current proximity value & threshold");
                Serial.println("status - Show current status");
                Serial.println("stars - Show total stars earned");
                Serial.println("set_stars <value> - Set star count");
                Serial.println("sleep_enable - Enable sleep mode");
                Serial.println("sleep_disable - Disable sleep mode");
                Serial.println("sleep_status - Show sleep status");
                Serial.println("sleep_now - Force enter light sleep");
                Serial.println("sleep_test - Test sleep mode with message display");
                Serial.println("stack_info - Show task stack usage and memory info");
            }
            commandBuffer = "";
        }
    } else {
        commandBuffer += c;
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