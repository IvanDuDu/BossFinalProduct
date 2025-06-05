#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

class ButtonManager {
private:
    int buttonPin;
    volatile bool buttonPressed;
    volatile unsigned long lastPressTime;
    volatile bool isHoldDetected;
    
    // Callback function pointers
    typedef void (*ButtonCallback)();
    ButtonCallback onClickCallback;
    ButtonCallback onHoldCallback;
    
    static ButtonManager* instance;

public:
    ButtonManager(int pin = 0);
    ~ButtonManager();

    // Initialization
    void begin();
    void setCallbacks(ButtonCallback onClick, ButtonCallback onHold);
    
    // Interrupt handler
    static void IRAM_ATTR handleInterrupt();
    void processButton();
    
    // Status functions
    bool isPressed();
};

extern ButtonManager buttonManager;

#endif