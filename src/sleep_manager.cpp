#include "sleep_manager.h"
#include "pet_state.h"
#include "persistent_storage.h"
#include "tasks.h"
#include "sleep_message.h"
#include "screens.h"
#include <esp_sleep.h>
#include <GxEPD2_BW.h>

// Global sleep manager instance
SleepManager sleepManager;

// External function declarations
extern void changePage(Page newPage);
extern void updateDisplay();
extern Page currentPage;
extern GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display;

SleepManager::SleepManager() :
    lastActivityTime(0),
    sleepEnabled(true),
    inLightSleep(false)
{
}

void SleepManager::begin() {
    // Configure GPIO39 as wake source
    esp_sleep_enable_ext0_wakeup(WAKE_PIN, 0); // Wake on LOW (button press)
    
    // Initialize activity timestamp
    lastActivityTime = millis();
    inLightSleep = false;
    
    Serial.println("Sleep manager initialized");
    Serial.print("Wake pin configured: GPIO");
    Serial.println(WAKE_PIN);
    Serial.print("Inactivity timeout: ");
    Serial.print(INACTIVITY_TIMEOUT_MS / 1000);
    Serial.println(" seconds");
}

void SleepManager::updateActivity() {
    lastActivityTime = millis();
    
    // If we were in light sleep, handle wake up
    if (inLightSleep) {
        Serial.println("Activity detected during sleep - waking up");
        inLightSleep = false;
        
        // Force full page refresh after wake (clears any display artifacts)
        forceFullRefresh();
    }
}

bool SleepManager::shouldEnterSleep() {
    if (!sleepEnabled || inLightSleep) {
        return false;
    }
    
    unsigned long currentTime = millis();
    unsigned long timeSinceActivity = currentTime - lastActivityTime;
    
    return timeSinceActivity >= INACTIVITY_TIMEOUT_MS;
}

void SleepManager::enterLightSleep() {
    if (inLightSleep || !sleepEnabled) {
        Serial.println("Sleep entry blocked - already sleeping or disabled");
        return;
    }
    
    Serial.println("Entering light sleep mode...");
    
    // Prepare for sleep - save pet state
    prepareForSleep();
    
    // Set flag before sleeping
    inLightSleep = true;
    
    // Enter light sleep
    Serial.println("ESP32 entering light sleep now...");
    esp_light_sleep_start();
    
    // When we wake up, this code will continue executing
    Serial.println("Waking from light sleep...");
    handleWakeFromSleep();
    inLightSleep = false;
    
    Serial.println("Sleep cycle completed");
}

bool SleepManager::isInLightSleep() {
    return inLightSleep;
}

void SleepManager::setSleepEnabled(bool enabled) {
    sleepEnabled = enabled;
    Serial.print("Sleep mode ");
    Serial.println(enabled ? "enabled" : "disabled");
}

unsigned long SleepManager::getTimeSinceLastActivity() {
    return millis() - lastActivityTime;
}

void SleepManager::setLastActivityTime(unsigned long time) {
    lastActivityTime = time;
}

void initializeSleepManager() {
    sleepManager.begin();
}

// Force a full screen refresh after wake from sleep
void forceFullRefresh() {
    Serial.println("Forcing full screen refresh after wake...");
    
    // Reset sleep message
    resetSleepMessage();
    
    // Full refresh - same as changePage function
    display.clearScreen();
    display.fillScreen(GxEPD_WHITE);
    
    // Draw the current page
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
    Serial.println("Full screen refresh completed");
}

void checkSleepConditions() {
    if (sleepManager.shouldEnterSleep()) {
        sleepManager.enterLightSleep();
    }
}

void prepareForSleep() {
    Serial.println("Preparing for sleep - saving pet state...");
    
    // Switch to home screen
    changePage(HOME_PAGE);
    
    // Show "Sleeping!" message
    showSleepMessageNow();
    
    // Update display to show the message
    updateDisplay();
    
    // Give time for the message to be displayed
    vTaskDelay(pdMS_TO_TICKS(2000)); // 2 seconds to show "Sleeping!" message
    
    // Save current pet state before sleeping
    if (xSemaphoreTake(petStateMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        // Send save event to storage task
        StorageEvent event = {StorageEvent::SAVE_ALL, 0};
        if (xQueueSend(storageQueue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
            Serial.println("Pet state save requested before sleep");
        } else {
            Serial.println("Failed to request pet state save before sleep");
        }
        xSemaphoreGive(petStateMutex);
    }
    
    // Give the storage task time to complete the save
    vTaskDelay(pdMS_TO_TICKS(500));
    
    Serial.println("Sleep preparation complete");
}

void handleWakeFromSleep() {
    Serial.println("Handling wake from sleep...");
    
    // Check wake cause
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wake caused by external signal using RTC_IO (encoder)");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wake caused by external signal using RTC_CNTL");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wake caused by timer");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wake caused by touchpad");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println("Wake caused by ULP program");
            break;
        default:
            Serial.printf("Wake was not caused by deep sleep: %d\n", wakeup_reason);
            break;
    }
    
    // Update activity time directly (avoid recursion)
    sleepManager.setLastActivityTime(millis());
    
    // Force full page refresh after wake (clears any display artifacts)
    forceFullRefresh();
    
    Serial.println("Wake handling complete");
} 