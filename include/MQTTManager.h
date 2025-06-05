#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTManager {
private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    unsigned long lastReconnectAttempt;
    
    // Callback function pointer
    typedef void (*MessageCallback)(char* topic, byte* payload, unsigned int length);
    MessageCallback messageCallback;

public:
    MQTTManager();
    ~MQTTManager();

    // MQTT connection management
    void begin(const char* server, int port);
    void setCallback(MessageCallback callback);
    bool connect(const char* token);
    void reconnect();
    void loop();
    
    // Message handling
    bool publish(const char* topic, const char* payload);
    bool subscribe(const char* topic);
    
    // Status functions
    bool isConnected();
    int getState();
};

extern MQTTManager mqttManager;

#endif