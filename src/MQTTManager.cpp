#include "MQTTManager.h"
#include "Config.h"

MQTTManager mqttManager;

MQTTManager::MQTTManager() : mqttClient(wifiClient), lastReconnectAttempt(0), messageCallback(nullptr) {
}

MQTTManager::~MQTTManager() {
    mqttClient.disconnect();
}

void MQTTManager::begin(const char* server, int port) {
    mqttClient.setServer(server, port);
    Serial.printf("MQTT server configured: %s:%d\n", server, port);
}

void MQTTManager::setCallback(MessageCallback callback) {
    messageCallback = callback;
    mqttClient.setCallback(callback);
}

bool MQTTManager::connect(const char* token) {
    String clientID = "ESP32Client-";

    clientID += String(random(0xffff), HEX);

    bool connected = false;
    if (strlen(token) > 0) {
        connected = mqttClient.connect(clientID.c_str(),token, "");
    } else {
        connected = mqttClient.connect(clientID.c_str(),mqtt_default_token,"");
    }

    if (connected) {
        Serial.println("Connected to MQTT! "+ String(token));
        
        // Subscribe to boss channel
        // Subscribe vào 4 kênh truyền của 4 staff
        if (strlen(bossName) > 0) {
            String topic = "User/" + String(bossName);
            subscribe(topic.c_str());
            subscribe("v1/devices/me/rpc/request/+");
        }
        
        lastReconnectAttempt = 0;
        return true;
    } else {
        Serial.print("MQTT connection failed, rc=");
        Serial.println(mqttClient.state());
        return false;
    }
}

void MQTTManager::reconnect() {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        
        if (!mqttClient.connected()) {
            Serial.print("Attempting MQTT connection...");
            if (connect(mqtt_default_token)) {
                lastReconnectAttempt = 0;
            }
        }
    }
}

void MQTTManager::loop() {
    if (!mqttClient.connected()) {
        reconnect();
    } else {
        mqttClient.loop();
    }
}

bool MQTTManager::publish(const char* topic, const char* payload) {
    if (mqttClient.connected()) {
        bool result = mqttClient.publish(topic, payload, true);
        if (result) {
            Serial.printf("MQTT published to %s: %s\n", topic, payload);
        } else {
            Serial.printf("MQTT publish failed to %s\n", topic);
        }
        return result;
    }
    return false;
}

bool MQTTManager::subscribe(const char* topic) {
    if (mqttClient.connected()) {
        bool result = mqttClient.subscribe(topic);
        if (result) {
            Serial.printf("MQTT subscribed to: %s\n", topic);
        } else {
            Serial.printf("MQTT subscribe failed to: %s\n", topic);
        }
        return result;
    }
    return false;
}

bool MQTTManager::isConnected() {
    return mqttClient.connected();
}

int MQTTManager::getState() {
    return mqttClient.state();
}