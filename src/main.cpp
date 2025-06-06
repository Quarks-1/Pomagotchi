#include <Arduino.h>
#include <USBCDC.h>

// Pin definitions for ESP32-S3
const int LED_PIN = 35;  // Built-in LED on ESP32-S3-DevKitC-1

void setup() {
    // Initialize USB CDC
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    Serial.println("ESP32-S3 Starting up...");

    // Initialize LED pin
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // Blink the LED
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    
    // Print status
    Serial.println("ESP32-S3 is running...");
} 