#include "persistent_storage.h"
#include <ArduinoJson.h>

bool initializeStorage() {
    Serial.println("Mounting LittleFS...");
    if (!LittleFS.begin(true)) {  // true for format if mount fails
        Serial.println("LittleFS mount failed");
        return false;
    }
    Serial.println("LittleFS mounted successfully");
    
    // Format the filesystem to start fresh
    Serial.println("Formatting LittleFS...");
    if (!LittleFS.format()) {
        Serial.println("Failed to format LittleFS");
        return false;
    }
    Serial.println("LittleFS formatted successfully");
    
    // Create initial config file with default values
    Serial.println("Creating initial config file with default values...");
    if (!saveValues(100, 100)) {
        Serial.println("Failed to create initial config file");
        return false;
    }
    Serial.println("Initial config file created successfully");
    
    return true;
}

bool saveValues(uint8_t sunlight, uint8_t thirst) {
    Serial.print("Saving values - Sunlight: ");
    Serial.print(sunlight);
    Serial.print(", Thirst: ");
    Serial.println(thirst);

    StaticJsonDocument<128> doc;  // Reduced from 200 to 128 since we only store two small values
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
    Serial.println("Successfully saved pet state");
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
    Serial.println("Loading sunlight value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 100");
        return 100;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 100;
    }

    StaticJsonDocument<128> doc;  // Reduced from 200 to 128
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file");
        return 100;
    }

    if (!doc.containsKey("sunlight")) {
        Serial.println("Sunlight key not found in config, using default value 100");
        return 100;
    }

    uint8_t value = doc["sunlight"].as<uint8_t>();
    Serial.print("Loaded sunlight value: ");
    Serial.println(value);
    return value;
}

uint8_t loadThirst() {
    Serial.println("Loading thirst value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 100");
        return 100;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 100;
    }

    StaticJsonDocument<128> doc;  // Reduced from 200 to 128
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file");
        return 100;
    }

    if (!doc.containsKey("thirst")) {
        Serial.println("Thirst key not found in config, using default value 100");
        return 100;
    }

    uint8_t value = doc["thirst"].as<uint8_t>();
    Serial.print("Loaded thirst value: ");
    Serial.println(value);
    return value;
} 