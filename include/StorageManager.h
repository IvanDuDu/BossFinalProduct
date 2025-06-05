#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Preferences.h>

class StorageManager {
private:
    Preferences preferences;

public:
    StorageManager();
    ~StorageManager();

    // EEPROM functions
    void clearEEPROM();
    void writeMACsToPROM(char MACaddress[][18],char tokenAddress[][80], int count);
    int readMACsFromPROM(char MACaddress[][18],char tokenAddress[][80]);

    // NVS functions
    void writeToNVS(const char* key, const char* value);
    String readFromNVS(const char* key);
    void clearWiFiCredentials();
    
    // Initialize storage
    void begin();
};

extern StorageManager storageManager;

#endif