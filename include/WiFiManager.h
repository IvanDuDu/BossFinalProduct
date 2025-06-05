#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class WiFiManager {
private:
    WiFiUDP udp;
    bool isConfigMode;

public:
    WiFiManager();
    ~WiFiManager();

    // WiFi connection management
    void begin();
    bool connectToWiFi(const char* ssid, const char* password);
    void startAPMode();
    void handleWiFiSetup();
    
    // UDP functions
    void beginUDP();
    bool processUDPPacket();
    void sendUDPResponse(const char* message);
    
    // Status functions
    bool isConnected();
    bool isInConfigMode();
    IPAddress getLocalIP();
    IPAddress getAPIP();
};

extern WiFiManager wifiManager;

#endif