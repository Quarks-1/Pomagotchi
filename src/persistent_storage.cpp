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
    if (!saveValues(100, 100, 10)) {  // Default: sunlight=100, thirst=100, petStatus=0
        Serial.println("Failed to create initial config file");
        return false;
    }
    Serial.println("Initial config file created successfully");
    
    return true;
}

bool saveValues(uint8_t sunlight, uint8_t thirst, uint8_t petStatus) {
    Serial.print("Saving values - Sunlight: ");
    Serial.print(sunlight);
    Serial.print(", Thirst: ");
    Serial.print(thirst);
    Serial.print(", PetStatus: ");
    Serial.println(petStatus);

    StaticJsonDocument<200> doc;  // Increased from 128 to 200 to accommodate petStatus
    doc["sunlight"] = sunlight;
    doc["thirst"] = thirst;
    doc["petStatus"] = petStatus;

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
    uint8_t currentPetStatus = loadPetStatus();
    return saveValues(value, currentThirst, currentPetStatus);
}

bool saveThirst(uint8_t value) {
    uint8_t currentSunlight = loadSunlight();
    uint8_t currentPetStatus = loadPetStatus();
    return saveValues(currentSunlight, value, currentPetStatus);
}

bool savePetStatus(uint8_t value) {
    uint8_t currentSunlight = loadSunlight();
    uint8_t currentThirst = loadThirst();
    return saveValues(currentSunlight, currentThirst, value);
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

    StaticJsonDocument<200> doc;  // Increased from 128 to 200
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

    StaticJsonDocument<200> doc;  // Increased from 128 to 200
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

uint8_t loadPetStatus() {
    Serial.println("Loading petStatus value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 0");
        return 0;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 0;
    }

    StaticJsonDocument<200> doc;  // Increased from 128 to 200
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file");
        return 0;
    }

    if (!doc.containsKey("petStatus")) {
        Serial.println("PetStatus key not found in config, using default value 0");
        return 0;
    }

    uint8_t value = doc["petStatus"].as<uint8_t>();
    Serial.print("Loaded petStatus value: ");
    Serial.println(value);
    return value;
} 