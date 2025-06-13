#include "tasks.h"
#include "main.h"
#include "encoder.h"
#include "pet_state.h"
#include "persistent_storage.h"
#include "screens.h"
#include "cursor.h"
#include "water_logging.h"
#include "sunbathing.h"
#include "pet_pommy.h"
#include "depletion.h"
#include "light_sensor.h"
#include "sleep_manager.h"

// External declarations
extern Encoder encoder;
extern bool isLightSensorEnabled;
extern void handleSerialInput(char c);
extern void updateDisplay();
extern bool saveValues(uint8_t sunlight, uint8_t thirst, uint8_t petStatus);

// Task handles
TaskHandle_t inputTaskHandle = NULL;
TaskHandle_t displayTaskHandle = NULL;
TaskHandle_t storageTaskHandle = NULL;
TaskHandle_t logicTaskHandle = NULL;
TaskHandle_t sleepTaskHandle = NULL;

// Queues
QueueHandle_t inputQueue = NULL;
QueueHandle_t displayUpdateQueue = NULL;
QueueHandle_t storageQueue = NULL;
QueueHandle_t logicQueue = NULL;

// Mutex for protecting shared data
SemaphoreHandle_t petStateMutex = NULL;
SemaphoreHandle_t displayMutex = NULL;

// Storage task variables
static uint8_t lastSavedSunlight = 0;
static uint8_t lastSavedThirst = 0;
static uint8_t lastSavedPetStatus = 0;
static unsigned long lastSaveTime = 0;
const unsigned long SAVE_INTERVAL = 30000; // Save every 30 seconds

void createTasks() {
    // Create queues
    inputQueue = xQueueCreate(10, sizeof(InputEvent));
    displayUpdateQueue = xQueueCreate(10, sizeof(DisplayUpdateEvent));
    storageQueue = xQueueCreate(5, sizeof(StorageEvent));
    logicQueue = xQueueCreate(5, sizeof(LogicEvent));
    
    // Create mutex
    petStateMutex = xSemaphoreCreateMutex();
    displayMutex = xSemaphoreCreateMutex();
    
    // Create tasks
    xTaskCreate(
        inputTask,
        "InputTask",
        INPUT_TASK_STACK_SIZE,
        NULL,
        INPUT_TASK_PRIORITY,
        &inputTaskHandle
    );
    
    xTaskCreate(
        displayTask,
        "DisplayTask",
        DISPLAY_TASK_STACK_SIZE,
        NULL,
        DISPLAY_TASK_PRIORITY,
        &displayTaskHandle
    );
    
    xTaskCreate(
        storageTask,
        "StorageTask",
        STORAGE_TASK_STACK_SIZE,
        NULL,
        STORAGE_TASK_PRIORITY,
        &storageTaskHandle
    );
    
    xTaskCreate(
        logicTask,
        "LogicTask",
        LOGIC_TASK_STACK_SIZE,
        NULL,
        LOGIC_TASK_PRIORITY,
        &logicTaskHandle
    );
    
    xTaskCreate(
        sleepTask,
        "SleepTask",
        SLEEP_TASK_STACK_SIZE,
        NULL,
        SLEEP_TASK_PRIORITY,
        &sleepTaskHandle
    );
}

void inputTask(void* parameter) {
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
            sleepManager.updateActivity(); // Update activity for sleep manager
        }
        
        // Handle encoder button
        if (encoder.getButtonPressed()) {
            InputEvent event = {InputEvent::ENCODER_BUTTON, 0};
            xQueueSend(inputQueue, &event, 0);
            encoder.resetButtonState();
            sleepManager.updateActivity(); // Update activity for sleep manager
        }
        
        // Handle serial input
        if (Serial.available()) {
            int readValue = Serial.read();
            if (readValue != -1) {  // Check if read was successful
                InputEvent event = {InputEvent::SERIAL_INPUT, static_cast<int16_t>(readValue)};
                xQueueSend(inputQueue, &event, 0);
                sleepManager.updateActivity(); // Update activity for sleep manager
            }
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
            }
            lastSensorRead = currentTime;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void displayTask(void* parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(100); // 100ms display update interval
    
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
                    // Handle serial input in a separate function
                    handleSerialInput(static_cast<char>(event.value));
                    break;
            }
        }
        
        // Update display
        if (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            updateDisplay();
            xSemaphoreGive(displayMutex);
        }
        
        vTaskDelay(xDelay);
    }
}

void storageTask(void* parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(5000); // Check every 5 seconds
    StorageEvent event;
    bool shouldSave = false;
    uint8_t currentSunlight = 0;
    uint8_t currentThirst = 0;
    uint8_t currentPetStatus = 0;
    
    while (1) {
        shouldSave = false;
        
        // Check for storage events
        if (xQueueReceive(storageQueue, &event, 0) == pdTRUE) {
            if (xSemaphoreTake(petStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                switch (event.type) {
                    case StorageEvent::SAVE_SUNLIGHT:
                        currentSunlight = event.value;
                        currentThirst = thirst;
                        currentPetStatus = petStatus;
                        shouldSave = true;
                        break;
                    case StorageEvent::SAVE_THIRST:
                        currentSunlight = sunlight;
                        currentThirst = event.value;
                        currentPetStatus = petStatus;
                        shouldSave = true;
                        break;
                    case StorageEvent::SAVE_PET_STATUS:
                        currentSunlight = sunlight;
                        currentThirst = thirst;
                        currentPetStatus = event.value;
                        shouldSave = true;
                        break;
                    case StorageEvent::SAVE_ALL:
                        currentSunlight = sunlight;
                        currentThirst = thirst;
                        currentPetStatus = petStatus;
                        shouldSave = true;
                        break;
                }
                xSemaphoreGive(petStateMutex);
            }
        }
        
        // Check for periodic save
        unsigned long currentTime = millis();
        if (!shouldSave && (currentTime - lastSaveTime >= SAVE_INTERVAL)) {
            if (xSemaphoreTake(petStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                if (lastSavedSunlight != sunlight || lastSavedThirst != thirst || lastSavedPetStatus != petStatus) {
                    currentSunlight = sunlight;
                    currentThirst = thirst;
                    currentPetStatus = petStatus;
                    shouldSave = true;
                }
                xSemaphoreGive(petStateMutex);
            }
        }
        
        // Perform save if needed
        if (shouldSave) {
            if (saveValues(currentSunlight, currentThirst, currentPetStatus)) {
                lastSavedSunlight = currentSunlight;
                lastSavedThirst = currentThirst;
                lastSavedPetStatus = currentPetStatus;
                lastSaveTime = currentTime;
                Serial.println("Pet state saved successfully");
            } else {
                Serial.println("Failed to save pet state");
            }
        }
        
        vTaskDelay(xDelay);
    }
}

void logicTask(void* parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(200); // Update every 200ms for better petting detection
    static unsigned long lastDepletionUpdate = 0;
    const unsigned long DEPLETION_UPDATE_INTERVAL = 1000; // Update depletion every second
    
    while (1) {
        if (xSemaphoreTake(petStateMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            unsigned long currentTime = millis();
            
            // Update depletion only every second (not every 200ms)
            if (currentTime - lastDepletionUpdate >= DEPLETION_UPDATE_INTERVAL) {
                updateDepletion(sunlight, thirst, petStatus, lastUpdateTime);
                lastDepletionUpdate = currentTime;
            }
            
            // Update sunlight level based on sensor readings
            if (isSunbathing) {
                updateSunlightLevel();
            }
            
            // Update petting detection based on proximity sensor (runs every 200ms)
            if (isPetting) {
                updatePettingDetection();
            }
            
            xSemaphoreGive(petStateMutex);
        }
        
        vTaskDelay(xDelay);
    }
}

void sleepTask(void* parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(SLEEP_CHECK_INTERVAL_MS); // Check every second
    
    while (1) {
        // Check if we should enter sleep mode
        checkSleepConditions();
        
        vTaskDelay(xDelay);
    }
} 