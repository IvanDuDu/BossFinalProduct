#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>

class RelayController {
private:
    int relayPins[4];
    int maxRelays;
    
public:
    RelayController();
    ~RelayController();

    // Initialization
    void begin();
    void setPins(const int* pins, int count);
    
    // Relay control
    void turnOn(int relayIndex);
    void turnOff(int relayIndex);
    void toggle(int relayIndex);
    void toggleWithDelay(int relayIndex, unsigned long delayMs = 5000);
    
    // Status functions
    bool isValidIndex(int relayIndex);
    int getMaxRelays();
};

extern RelayController relayController;

#endif