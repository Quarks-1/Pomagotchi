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

bool saveValues(uint8_t sunlight, uint8_t thirst, uint8_t petStatus, uint32_t stars) {
    Serial.print("Saving values - Sunlight: ");
    Serial.print(sunlight);
    Serial.print(", Thirst: ");
    Serial.print(thirst);
    Serial.print(", PetStatus: ");
    Serial.println(petStatus);
    Serial.print("Stars: ");
    Serial.println(stars);

    StaticJsonDocument<512> doc;  // Increased to 512 to accommodate hats data
    
    // Load existing JSON to preserve all existing data (especially hats)
    if (LittleFS.exists(CONFIG_FILE)) {
        File file = LittleFS.open(CONFIG_FILE, "r");
        if (file) {
            DeserializationError error = deserializeJson(doc, file);
            file.close();
            if (error) {
                Serial.println("Failed to parse existing config file, creating new one");
                doc.clear(); // Start fresh if parsing fails
            }
        }
    }
    
    // Update the pet state values
    doc["sunlight"] = sunlight;
    doc["thirst"] = thirst;
    doc["petStatus"] = petStatus;
    doc["stars"] = stars;
    
    // Ensure hats structure exists with defaults if not already present
    if (!doc.containsKey("hats")) {
        JsonObject hats = doc.createNestedObject("hats");
        JsonObject topHat = hats.createNestedObject("topHat");
        topHat["purchased"] = false;
        topHat["wearing"] = false;
        JsonObject cowboyHat = hats.createNestedObject("cowboyHat");
        cowboyHat["purchased"] = false;
        cowboyHat["wearing"] = false;
        JsonObject partyHat = hats.createNestedObject("partyHat");
        partyHat["purchased"] = false;
        partyHat["wearing"] = false;
        JsonObject starHat = hats.createNestedObject("starHat");
        starHat["purchased"] = false;
        starHat["wearing"] = false;
    }

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
    Serial.println("Successfully saved pet state with hats structure");
    return true;
}

bool saveSunlight(uint8_t value) {
    Serial.print("Saving sunlight: ");
    Serial.println(value);

    StaticJsonDocument<512> doc;
    
    // Load existing JSON to preserve all existing data
    if (LittleFS.exists(CONFIG_FILE)) {
        File file = LittleFS.open(CONFIG_FILE, "r");
        if (file) {
            DeserializationError error = deserializeJson(doc, file);
            file.close();
            if (error) {
                Serial.println("Failed to parse existing config file for sunlight save");
                return false;
            }
        } else {
            Serial.println("Failed to open config file for reading sunlight");
            return false;
        }
    } else {
        Serial.println("Config file not found for sunlight save");
        return false;
    }
    
    // Update only the sunlight value
    doc["sunlight"] = value;

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Successfully saved sunlight");
    return true;
}

bool saveThirst(uint8_t value) {
    Serial.print("Saving thirst: ");
    Serial.println(value);

    StaticJsonDocument<512> doc;
    
    // Load existing JSON to preserve all existing data
    if (LittleFS.exists(CONFIG_FILE)) {
        File file = LittleFS.open(CONFIG_FILE, "r");
        if (file) {
            DeserializationError error = deserializeJson(doc, file);
            file.close();
            if (error) {
                Serial.println("Failed to parse existing config file for thirst save");
                return false;
            }
        } else {
            Serial.println("Failed to open config file for reading thirst");
            return false;
        }
    } else {
        Serial.println("Config file not found for thirst save");
        return false;
    }
    
    // Update only the thirst value
    doc["thirst"] = value;

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Successfully saved thirst");
    return true;
}

bool savePetStatus(uint8_t value) {
    Serial.print("Saving petStatus: ");
    Serial.println(value);

    StaticJsonDocument<512> doc;
    
    // Load existing JSON to preserve all existing data
    if (LittleFS.exists(CONFIG_FILE)) {
        File file = LittleFS.open(CONFIG_FILE, "r");
        if (file) {
            DeserializationError error = deserializeJson(doc, file);
            file.close();
            if (error) {
                Serial.println("Failed to parse existing config file for petStatus save");
                return false;
            }
        } else {
            Serial.println("Failed to open config file for reading petStatus");
            return false;
        }
    } else {
        Serial.println("Config file not found for petStatus save");
        return false;
    }
    
    // Update only the petStatus value
    doc["petStatus"] = value;

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Successfully saved petStatus");
    return true;
}

bool saveStars(uint32_t stars) {
    Serial.print("Saving stars: ");
    Serial.println(stars);

    StaticJsonDocument<512> doc;
    
    // Load existing JSON to preserve all existing data
    if (LittleFS.exists(CONFIG_FILE)) {
        File file = LittleFS.open(CONFIG_FILE, "r");
        if (file) {
            DeserializationError error = deserializeJson(doc, file);
            file.close();
            if (error) {
                Serial.println("Failed to parse existing config file for stars save");
                return false;
            }
        } else {
            Serial.println("Failed to open config file for reading stars");
            return false;
        }
    } else {
        Serial.println("Config file not found for stars save");
        return false;
    }
    
    // Update only the stars value
    doc["stars"] = stars;

    File file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Successfully saved stars");
    return true;
}

uint8_t loadSunlight() {
    // Serial.println("Loading sunlight value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 100");
        return 100;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 100;
    }

    StaticJsonDocument<512> doc;  // Increased to 512 to accommodate hats data
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
    // Serial.println("Loading thirst value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 100");
        return 100;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 100;
    }

    StaticJsonDocument<512> doc;  // Increased to 512 to accommodate hats data
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
    // Serial.println("Loading petStatus value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 0");
        return 0;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 0;
    }

    StaticJsonDocument<512> doc;  // Increased to 512 to accommodate hats data
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
    // Serial.print("Loaded petStatus value: ");
    Serial.println(value);
    return value;
}

uint32_t loadStars() {
    // Serial.println("Loading stars value...");
    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, using default value 0");
        return 0;  // Default value
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return 0;
    }

    StaticJsonDocument<512> doc;
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

// Helper function to map hat type enum to JSON key
const char* getHatJsonKey(uint8_t hatType) {
    switch (hatType) {
        case 1: return "topHat";        // TOPHAT
        case 2: return "cowboyHat";     // COWBOY_HAT
        case 3: return "partyHat";      // PARTY_HAT
        case 4: return "starHat";       // STAR_HAT
        default: return nullptr;
    }
}

bool saveHatPurchased(uint8_t hatType, bool purchased) {
    const char* hatKey = getHatJsonKey(hatType);
    if (!hatKey) {
        Serial.println("Invalid hat type for saveHatPurchased");
        return false;
    }
    
    // Serial.print("Saving hat purchased status: ");
    // Serial.print(hatKey);
    Serial.print(" = ");
    Serial.println(purchased);

    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found for hat save");
        return false;
    }

    // Load existing data
    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return false;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file for hat save");
        return false;
    }

    // Ensure hats object exists
    if (!doc.containsKey("hats")) {
        doc.createNestedObject("hats");
    }
    
    // Ensure specific hat object exists
    if (!doc["hats"].containsKey(hatKey)) {
        JsonObject hat = doc["hats"].createNestedObject(hatKey);
        hat["purchased"] = false;
        hat["wearing"] = false;
    }
    
    // Update purchased status
    doc["hats"][hatKey]["purchased"] = purchased;

    // Save back to file
    file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file for hat save");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Successfully saved hat purchased status");
    return true;
}

bool saveHatWearing(uint8_t hatType, bool wearing) {
    const char* hatKey = getHatJsonKey(hatType);
    if (!hatKey) {
        Serial.println("Invalid hat type for saveHatWearing");
        return false;
    }
    
    Serial.print("Saving hat wearing status: ");
    Serial.print(hatKey);
    Serial.print(" = ");
    Serial.println(wearing);

    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found for hat save");
        return false;
    }

    // Load existing data
    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return false;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file for hat save");
        return false;
    }

    // Ensure hats object exists
    if (!doc.containsKey("hats")) {
        doc.createNestedObject("hats");
    }
    
    // Ensure specific hat object exists
    if (!doc["hats"].containsKey(hatKey)) {
        JsonObject hat = doc["hats"].createNestedObject(hatKey);
        hat["purchased"] = false;
        hat["wearing"] = false;
    }
    
    // Update wearing status
    doc["hats"][hatKey]["wearing"] = wearing;

    // Save back to file
    file = LittleFS.open(CONFIG_FILE, "w");
    if (!file) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(doc, file) == 0) {
        Serial.println("Failed to write config file for hat save");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Successfully saved hat wearing status");
    return true;
}

bool loadHatPurchased(uint8_t hatType) {
    const char* hatKey = getHatJsonKey(hatType);
    if (!hatKey) {
        Serial.println("Invalid hat type for loadHatPurchased");
        return false;
    }
    
    // Serial.print("Loading hat purchased status: ");
    Serial.println(hatKey);

    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, hat not purchased");
        return false;
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return false;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file for hat load");
        return false;
    }

    if (!doc.containsKey("hats") || !doc["hats"].containsKey(hatKey) || !doc["hats"][hatKey].containsKey("purchased")) {
        Serial.println("Hat purchased status not found, defaulting to false");
        return false;
    }

    bool purchased = doc["hats"][hatKey]["purchased"].as<bool>();
    // Serial.print("Hat purchased status: ");
    // Serial.println(purchased);
    return purchased;
}

bool loadHatWearing(uint8_t hatType) {
    const char* hatKey = getHatJsonKey(hatType);
    if (!hatKey) {
        Serial.println("Invalid hat type for loadHatWearing");
        return false;
    }
    
    // Serial.print("Loading hat wearing status: ");
    // Serial.println(hatKey);

    if (!LittleFS.exists(CONFIG_FILE)) {
        Serial.println("Config file not found, hat not wearing");
        return false;
    }

    File file = LittleFS.open(CONFIG_FILE, "r");
    if (!file) {
        Serial.println("Failed to open config file for reading");
        return false;
    }

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Failed to parse config file for hat load");
        return false;
    }

    if (!doc.containsKey("hats") || !doc["hats"].containsKey(hatKey) || !doc["hats"][hatKey].containsKey("wearing")) {
        Serial.println("Hat wearing status not found, defaulting to false");
        return false;
    }

    bool wearing = doc["hats"][hatKey]["wearing"].as<bool>();
    // Serial.print("Hat wearing status: ");
    // Serial.println(wearing);
    return wearing;
} 