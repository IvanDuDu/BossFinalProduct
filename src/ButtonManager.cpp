#include "ButtonManager.h"
#include "Config.h"

ButtonManager buttonManager(CLEAR_BUTTON_PIN);
ButtonManager* ButtonManager::instance = nullptr;

ButtonManager::ButtonManager(int pin) : 
    buttonPin(pin), 
    buttonPressed(false), 
    lastPressTime(0), 
    isHoldDetected(false),
    onClickCallback(nullptr),
    onHoldCallback(nullptr) {
    instance = this;
}

ButtonManager::~ButtonManager() {
    detachInterrupt(digitalPinToInterrupt(buttonPin));
}

void ButtonManager::begin() {
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, CHANGE);
    Serial.printf("Button manager initialized on pin %d\n", buttonPin);
}

void ButtonManager::setCallbacks(ButtonCallback onClick, ButtonCallback onHold) {
    onClickCallback = onClick;
    onHoldCallback = onHold;
}

void IRAM_ATTR ButtonManager::handleInterrupt() {
    if (instance) {
        instance->processButton();
    }
}

void ButtonManager::processButton() {
    bool currentState = digitalRead(buttonPin);
    
    if (currentState == LOW && !buttonPressed) {
        // Button pressed
        lastPressTime = millis();
        buttonPressed = true;
        isHoldDetected = false;
    } else if (currentState == HIGH && buttonPressed) {
        // Button released
        unsigned long pressDuration = millis() - lastPressTime;
        buttonPressed = false;
        
        if (pressDuration >= 3000) {
            // Long press (hold)
            isHoldDetected = true;
            Serial.println("Button HOLD detected");
            if (onHoldCallback) {
                onHoldCallback();
            }
        } else if (pressDuration >= 50) {
            // Short press (click) - debounce with 50ms minimum
            Serial.println("Button CLICK detected");
            if (onClickCallback) {
                onClickCallback();
            }
        }
        
        lastPressTime = 0;
    }
}

bool ButtonManager::isPressed() {
    return buttonPressed;
}