#include "persistent_storage.h"
#include <ArduinoJson.h>

bool initializeStorage() {
    Serial.println("Mounting LittleFS...");
    if (!LittleFS.begin()) {  // Try mounting without formatting first
        Serial.println("LittleFS mount failed, attempting to format...");
        if (!LittleFS.begin(true)) {  // true for format if mount fails
            Serial.println("LittleFS mount failed even after formatting");
            return false;
        }
        Serial.println("LittleFS formatted and mounted successfully");
    } else {
        Serial.println("LittleFS mounted successfully");
    }
    
    // Only create initial config file if it doesn't exist
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, creating with default values...");
        if (!saveValues(100, 100, 10, 0)) {  // Default: sunlight=100, thirst=100, petStatus=10, stars=0
            Serial.println("Failed to create initial config file");
            return false;
        }
        Serial.println("Initial config file created successfully");
    }
    
    return true;
}

bool saveValues(uint8_t sunlight, uint8_t thirst, uint8_t petStatus, uint8_t stars) {
    Serial.print("Saving values - Sunlight: ");
    Serial.print(sunlight);
    Serial.print(", Thirst: ");
    Serial.print(thirst);
    Serial.print(", PetStatus: ");
    Serial.println(petStatus);
    Serial.print("Stars: ");
    Serial.println(stars);

    StaticJsonDocument<200> doc;  // Increased from 128 to 200 to accommodate petStatus
    doc["sunlight"] = sunlight;
    doc["thirst"] = thirst;
    doc["petStatus"] = petStatus;
    doc["stars"] = stars; // Preserve star count when saving other values

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
    uint8_t currentStars = loadStars();
    return saveValues(value, currentThirst, currentPetStatus, currentStars);
}

bool saveThirst(uint8_t value) {
    uint8_t currentSunlight = loadSunlight();
    uint8_t currentPetStatus = loadPetStatus();
    uint8_t currentStars = loadStars();
    return saveValues(currentSunlight, value, currentPetStatus, currentStars);
}

bool savePetStatus(uint8_t value) {
    uint8_t currentSunlight = loadSunlight();
    uint8_t currentThirst = loadThirst();
    uint8_t currentStars = loadStars();
    return saveValues(currentSunlight, currentThirst, value, currentStars);
}

bool saveStars(uint32_t stars) {
    Serial.print("Saving stars: ");
    Serial.println(stars);

    StaticJsonDocument<200> doc;
    
    // Load existing values
    doc["sunlight"] = loadSunlight();
    doc["thirst"] = loadThirst();
    doc["petStatus"] = loadPetStatus();
    doc["stars"] = stars;  // Use the new stars value passed to the function

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
    Serial.println("Successfully saved stars");
    return true;
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

uint32_t loadStars() {
    Serial.println("Loading stars value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 0");
        return 0;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 0;
    }

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file");
        return 0;
    }

    if (!doc.containsKey("stars")) {
        Serial.println("Stars key not found in config, using default value 0");
        return 0;
    }

    uint32_t value = doc["stars"].as<uint32_t>();
    Serial.print("Loaded stars value: ");
    Serial.println(value);
    return value;
} 