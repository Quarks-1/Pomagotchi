#include "encoder.h"

Encoder::Encoder() : 
    encoder_position(0),
    last_position(0),
    button_pressed(false),
    last_button_state(false),
    isEncoderEnabled(false)
{
}

void Encoder::begin() {
    if (!ss.begin(SEESAW_ADDR)) {
        Serial.println("Couldn't find encoder on I2C bus! Encoder functionality will be disabled.");
        isEncoderEnabled = false;
        return;
    }
    
    // Set up encoder
    ss.pinMode(ENCODER_BUTTON, INPUT_PULLUP);
    ss.setGPIOInterrupts((uint32_t)1 << ENCODER_BUTTON, 1);
    ss.enableEncoderInterrupt();
    isEncoderEnabled = true;
    Serial.println("Encoder initialized successfully!");
}

void Encoder::update() {
    // Read encoder position
    int32_t new_position = -ss.getEncoderPosition();
    int32_t delta = new_position - last_position;
    last_position = new_position;
    
    // Read button state with debouncing
    bool current_button = !ss.digitalRead(ENCODER_BUTTON);
    if (current_button != last_button_state) {
        // Simple debounce - only trigger on rising edge (button press)
        if (current_button) {
            button_pressed = true;
            Serial.println("Button pressed!");
        }
        last_button_state = current_button;
    }
}

int8_t Encoder::getDelta() {
    int32_t delta = (last_position - encoder_position)*-1;
    encoder_position = last_position;
    if (delta != 0) {
        Serial.print("Encoder turned: ");
        Serial.print(delta > 0 ? "clockwise" : "counter-clockwise");
        Serial.print(" (delta: ");
        Serial.print(delta);
        Serial.println(")");
    }
    return delta;
}

bool Encoder::getButtonPressed() {
    return button_pressed;
}

void Encoder::resetButtonState() {
    button_pressed = false;
} 