#include "RelayController.h"
#include "Config.h"

RelayController relayController;

RelayController::RelayController() : maxRelays(MAX_PIN) {
    // Copy default pins
    for (int i = 0; i < MAX_PIN; i++) {
        relayPins[i] = RELAY_PINS[i];
    }
}

RelayController::~RelayController() {
}

void RelayController::begin() {
    for (int i = 0; i < maxRelays; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], LOW);
    }
    Serial.printf("Relay controller initialized with %d relays\n", maxRelays);
}

void RelayController::setPins(const int* pins, int count) {
    if (count <= 4) {
        maxRelays = count;
        for (int i = 0; i < count; i++) {
            relayPins[i] = pins[i];
        }
    }
}

void RelayController::turnOn(int relayIndex) {
    if (isValidIndex(relayIndex)) {
        digitalWrite(relayPins[relayIndex], HIGH);
        Serial.printf("Relay %d ON\n", relayIndex);
    }
}

void RelayController::turnOff(int relayIndex) {
    if (isValidIndex(relayIndex)) {
        digitalWrite(relayPins[relayIndex], LOW);
        Serial.printf("Relay %d OFF\n", relayIndex);
    }
}

void RelayController::toggle(int relayIndex) {
    if (isValidIndex(relayIndex)) {
        int currentState = digitalRead(relayPins[relayIndex]);
        digitalWrite(relayPins[relayIndex], !currentState);
        Serial.printf("Relay %d toggled to %s\n", relayIndex, !currentState ? "ON" : "OFF");
    }
}

void RelayController::toggleWithDelay(int relayIndex, unsigned long delayMs) {
    if (isValidIndex(relayIndex)) {
        turnOn(relayIndex);
        delay(delayMs);
        turnOff(relayIndex);
    }
}

bool RelayController::isValidIndex(int relayIndex) {
    if (relayIndex >= 0 && relayIndex < maxRelays) {
        return true;
    }
    Serial.printf("Invalid relay index: %d (max: %d)\n", relayIndex, maxRelays - 1);
    return false;
}

int RelayController::getMaxRelays() {
    return maxRelays;
}