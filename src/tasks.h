#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

// Task priorities
#define INPUT_TASK_PRIORITY 3
#define DISPLAY_TASK_PRIORITY 2
#define STORAGE_TASK_PRIORITY 1
#define LOGIC_TASK_PRIORITY 2

// Task stack sizes
#define INPUT_TASK_STACK_SIZE 4096
#define DISPLAY_TASK_STACK_SIZE 4096
#define STORAGE_TASK_STACK_SIZE 4096
#define LOGIC_TASK_STACK_SIZE 2048

// Task handles
extern TaskHandle_t inputTaskHandle;
extern TaskHandle_t displayTaskHandle;
extern TaskHandle_t storageTaskHandle;
extern TaskHandle_t logicTaskHandle;

// Queues
extern QueueHandle_t inputQueue;
extern QueueHandle_t displayUpdateQueue;
extern QueueHandle_t storageQueue;
extern QueueHandle_t logicQueue;

// Mutex for protecting shared data
extern SemaphoreHandle_t petStateMutex;

// Input event structure
struct InputEvent {
    enum Type {
        ENCODER_DELTA,
        ENCODER_BUTTON,
        SERIAL_INPUT
    } type;
    int16_t value;
};

// Display update event structure
struct DisplayUpdateEvent {
    enum Type {
        PAGE_CHANGE,
        CURSOR_UPDATE,
        ANIMATION_UPDATE,
        STATUS_UPDATE
    } type;
    void* data;
};

// Storage event structure
struct StorageEvent {
    enum Type {
        SAVE_SUNLIGHT,
        SAVE_THIRST,
        SAVE_ALL
    } type;
    uint8_t value;
};

// Logic event structure
struct LogicEvent {
    enum Type {
        UPDATE_DEPLETION,
        UPDATE_SUNLIGHT,
        UPDATE_THIRST
    } type;
    void* data;
};

// Function declarations
void createTasks();
void inputTask(void* parameter);
void displayTask(void* parameter);
void storageTask(void* parameter);
void logicTask(void* parameter); 