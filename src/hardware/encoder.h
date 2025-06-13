#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_seesaw.h>

class Encoder {
public:
    Encoder();
    void begin();
    void update();
    int8_t getDelta();
    bool getButtonPressed();
    void resetButtonState();
    bool isEnabled() { return isEncoderEnabled; }

private:
    Adafruit_seesaw ss;
    int32_t encoder_position;
    int32_t last_position;
    bool button_pressed;
    bool last_button_state;
    bool isEncoderEnabled;
    static const uint8_t SEESAW_ADDR = 0x36;  // Default I2C address for QT Stemma
    static const uint8_t ENCODER_BUTTON = 24;  // Button pin on Seesaw
}; 