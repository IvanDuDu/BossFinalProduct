#include <Arduino.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "StorageManager.h"
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "RelayController.h"
#include "ButtonManager.h"

// Forward declarations
void onMQTTMessage(char* topic, byte* payload, unsigned int length);
void onButtonClick();
void onButtonHold();
void processUDPSensorData();
bool isValidMAC(const char* mac);
int  findMACIndex(const char* mac);
void addNewDevice(const char* mac,const char * token);
WiFiUDP udp;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== ESP32 BossVerse System Starting ===");
    
    // Initialize storage manager
    storageManager.begin();
    
    // Load MAC addresses from EEPROM
    count_relayOn = storageManager.readMACsFromPROM(MACaddress,tokenAddress);
    
    // Print loaded MAC addresses
    Serial.println("Loaded MAC addresses:");
    for (int i = 0; i < MAX_PIN; i++) {
        if (strlen(MACaddress[i]) > 0) {
            Serial.printf("  [%d]: %s\n", i, MACaddress[i]);
        }
    }

    Serial.println("Loaded token addresses:");
    for (int i = 0; i < MAX_PIN; i++) {
        if (strlen(tokenAddress[i]) > 0) {
            Serial.printf("  [%d]: %s\n", i, tokenAddress[i]);
        }
    }
    
    // Initialize relay controller
    relayController.begin();
    
    // Initialize button manager with callbacks
    buttonManager.setCallbacks(onButtonClick, onButtonHold);
    buttonManager.begin();
    
    // Initialize WiFi manager
    wifiManager.begin();
    
    // Initialize MQTT if we have valid credentials
    if (strlen(mqtt_server) > 0) {
        mqttManager.begin(mqtt_server, mqtt_port);
        mqttManager.setCallback(onMQTTMessage);
    }
    
    udp.begin(1234);
    Serial.println("=== System initialization complete ===\n");
}

void loop() {
    // Handle MQTT
        mqttManager.loop();
    
    // Process UDP packets for sensor data and configuration
    processUDPSensorData();
    
    // Small delay to prevent watchdog issues
    delay(10);
}

// // MQTT message callback
// void onMQTTMessage(char* topic, byte* payload, unsigned int length) {
//     String message = "";
//     for (int i = 0; i < length; i++) {
//         message += (char)payload[i];
//     }
    
//     Serial.printf("MQTT message from %s: %s\n", topic, message.c_str());
    
//     // Process relay control commands
//     for (int i = 0; i < length; i++) {
//         if (isDigit(payload[i])) {
//             int relayIndex = payload[i] - '0';
//             if (relayIndex < relayController.getMaxRelays()) {
//                 relayController.toggleWithDelay(relayIndex);
//             }
//         }
//     }
// }

void onMQTTMessage(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.printf("MQTT message from %s: %s\n", topic, message.c_str());

    // Kiểm tra topic có phải là RPC request không
    if (String(topic).startsWith("v1/devices/me/rpc/request/")) {
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, message);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }

        const char* method = doc["method"];
        const char* bossID = doc["params"]["bossID"];
        const char* deviceToken = doc["params"]["device"];

        Serial.printf("Received RPC:\n Method: %s\n BossID: %s\n DeviceToken: %s\n", 
                      method, bossID, deviceToken);

        // Ví dụ: xử lý tùy theo method
        if (strcmp(bossID,bossName)==0) {
           int relayIndex = findMACIndex(deviceToken);
            relayController.toggleWithDelay(relayIndex);
        }
    } 
    else {
        // Xử lý bình thường nếu là các topic khác
        for (int i = 0; i < length; i++) {
            if (isDigit(payload[i])) {
                int relayIndex = payload[i] - '0';
                if (relayIndex < relayController.getMaxRelays()) {
                    relayController.toggleWithDelay(relayIndex);
                }
            }
        }
    }
}



// Button click callback - clear WiFi credentials
void onButtonClick() {
    Serial.println("Clearing WiFi credentials...");
    storageManager.clearWiFiCredentials();
}

// Button hold callback - clear EEPROM
void onButtonHold() {
    Serial.println("Clearing EEPROM data...");
    storageManager.clearEEPROM();
    count_relayOn = 0;
    
    // Clear MAC address array
    for (int i = 0; i < MAX_PIN; i++) {
        memset(MACaddress[i], 0, 18);
    }
}

// Process UDP packets for sensor data
void processUDPSensorData() {
    

    // Check for UDP packets
    int packetSize = udp.parsePacket();
    
    if (packetSize > 0) {
        char packetBuffer[256];
        int len = udp.read(packetBuffer, 255);
        if (len > 0) packetBuffer[len] = '\0';
        
        Serial.print("Received UDP packet: ");
        Serial.println(packetBuffer);
        
        // Parse packet: MAC//Token//VALUE
        char* mac = strtok(packetBuffer, "//");
        char* token = strtok(NULL, "//");
        char* data = strtok(NULL, "//");

        
        if (mac && data) {
            if (strcmp(data, "Setting") == 0) {
                // Handle configuration request
                Serial.println("Configuration request received");
                // Send current configuration back
                udp.beginPacket(udp.remoteIP(), udp.remotePort());
                udp.print("Config response");
                udp.endPacket();
            } else {
                // Handle sensor data
                int sensorValue = atoi(data);
                
                // Create JSON response
                DynamicJsonDocument doc(512);
                doc["bossID"] = bossName;
              
                doc["value"] = sensorValue;
                
                // Check if sensor value exceeds threshold
                
                    int macIndex = findMACIndex(mac);
                    if (macIndex >= 0) {
                        doc["targetDeviceId"] = tokenAddress[macIndex] ;
                        if (sensorValue > 400) {
                        // Known device - toggle relay
                        relayController.toggleWithDelay(macIndex);
                      
                       doc["action"] = "relay_triggered";
                    } else {
                        doc["action"] = "value_below_threshold";
                    }
                } else {
                    // New device - add to memory and EEPROM
                        if (count_relayOn < MAX_PIN) {
                            addNewDevice(mac, token);
                      //      doc["deviceID"] = count_relayOn - 1;
                            doc["action"] = "device_added";
                        } else {
                            doc["action"] = "device_limit_reached";
                        }
                }
                
                // Send UDP response
                udp.beginPacket(udp.remoteIP(), udp.remotePort());
                udp.print("ACK");
                udp.endPacket();

                // Publish to MQTT if connected
                if (mqttManager.isConnected()) {
                    char mqttMessage[256];
                    serializeJson(doc, mqttMessage);
                    mqttManager.publish("v1/devices/me/telemetry", mqttMessage);
                }
            }
        }
    }
}

// Check if MAC address is valid format
bool isValidMAC(const char* mac) {
    if (strlen(mac) != 17) return false;
    
    for (int i = 0; i < 17; i++) {
        if (i % 3 == 2) {
            if (mac[i] != ':') return false;
        } else {
            if (!isxdigit(mac[i])) return false;
        }
    }
    return true;
}

// Find MAC address index in array
int findMACIndex(const char* mac) {
    //if (!isValidMAC(mac)) return -1;
    
    for (int i = 0; i < MAX_PIN; i++) {
        if (strcmp(MACaddress[i], mac) == 0 
        || strcmp(tokenAddress[i], mac) == 0 ) {
            return i;
        }
    }
    return -1;
}

// Add new device MAC to memory and EEPROM
void addNewDevice(const char* mac, const char* token) {
    if (!isValidMAC(mac) || count_relayOn >= MAX_PIN) {
        Serial.println("Cannot add device: invalid MAC or limit reached");
        return;
    }
    
    Serial.printf("Adding new device: %s at index %d,token=%s\n", mac, count_relayOn,token);
    strcpy(MACaddress[count_relayOn], mac);
    strcpy(tokenAddress[count_relayOn], token);

    count_relayOn++;
    
    // Save to EEPROM
    storageManager.writeMACsToPROM(MACaddress,tokenAddress, count_relayOn);

}