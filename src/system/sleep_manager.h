#pragma once

#include <Arduino.h>
#include <esp_sleep.h>

// Sleep configuration constants
#define INACTIVITY_TIMEOUT_MS (5 * 60 * 1000)  // 5 minutes in milliseconds
#define WAKE_PIN GPIO_NUM_39                   // GPIO39 for encoder INT pin
#define SLEEP_CHECK_INTERVAL_MS 1000           // Check sleep conditions every second

// Forward declarations
void forceFullRefresh();

// Sleep manager class
class SleepManager {
private:
    unsigned long lastActivityTime;
    unsigned long sleepStartTime;  // Track when sleep mode begins
    bool sleepEnabled;
    bool inLightSleep;
    
    // Make handleWakeFromSleep a private member function
    void handleWakeFromSleep();
    
public:
    SleepManager();
    
    // Initialize sleep management
    void begin();
    
    // Update activity timestamp (call whenever there's user input)
    void updateActivity();
    
    // Check if we should enter light sleep
    bool shouldEnterSleep();
    
    // Enter light sleep mode
    void enterLightSleep();
    
    // Check if we're currently in light sleep
    bool isInLightSleep();
    
    // Enable/disable sleep functionality
    void setSleepEnabled(bool enabled);
    
    // Get time since last activity in milliseconds
    unsigned long getTimeSinceLastActivity();
    
    // Update activity time directly (used internally to avoid recursion)
    void setLastActivityTime(unsigned long time);
    
    // Get the time spent in sleep mode (in milliseconds)
    unsigned long getSleepDuration();
    
    // Get the sleep start time
    unsigned long getSleepStartTime() const { return sleepStartTime; }
};

// Global sleep manager instance
extern SleepManager sleepManager;

// Function declarations
void initializeSleepManager();
void checkSleepConditions();
void prepareForSleep();
void handleWakeFromSleep(); 