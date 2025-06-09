#include "persistent_storage.h"
#include <ArduinoJson.h>

bool initializeStorage() {
    if (!LittleFS.begin(true)) {  // true for format if mount fails
        Serial.println("LittleFS mount failed");
        return false;
    }
    return true;
}

bool saveValues(uint8_t sunlight, uint8_t thirst) {
    StaticJsonDocument<200> doc;
    doc["sunlight"] = sunlight;
    doc["thirst"] = thirst;

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write to config file");
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool saveSunlight(uint8_t value) {
    uint8_t currentThirst = loadThirst();
    return saveValues(value, currentThirst);
}

bool saveThirst(uint8_t value) {
    uint8_t currentSunlight = loadSunlight();
    return saveValues(currentSunlight, value);
}

uint8_t loadSunlight() {
    if (!LittleFS.exists(CONFIG_FILE)) {
        return 100;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 100;
    }

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file");
        return 100;
    }

    return doc["sunlight"] | 100;  // Use 100 as default if not found
}

uint8_t loadThirst() {
    if (!LittleFS.exists(CONFIG_FILE)) {
        return 100;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 100;
    }

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file");
        return 100;
    }

    return doc["thirst"] | 100;  // Use 100 as default if not found
} 