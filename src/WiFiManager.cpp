#include "WiFiManager.h"
#include "Config.h"
#include "StorageManager.h"

WiFiManager wifiManager;

WiFiManager::WiFiManager() : isConfigMode(false) {
}

WiFiManager::~WiFiManager() {
    udp.stop();
}

void WiFiManager::begin() {
    WiFi.mode(WIFI_AP_STA);
    handleWiFiSetup();
}

bool WiFiManager::connectToWiFi(const char* ssid, const char* password) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
        delay(500);
        Serial.print(".");
        retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected.");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        isConfigMode = false;
        return true;
    } else {
        Serial.println("\nWiFi connection failed.");
        return false;
    }
}

void WiFiManager::startAPMode() {
    Serial.println("Starting AP mode...");
    WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    isConfigMode = true;
}

void WiFiManager::handleWiFiSetup() {
    String storedSSID = storageManager.readFromNVS("ssid");
    String storedPassword = storageManager.readFromNVS("password");
    String storedBossName = storageManager.readFromNVS("bossName");
    String storedBossID = storageManager.readFromNVS("bossID");

    if (storedSSID == "" || storedPassword == "") {
        Serial.println("No stored WiFi credentials, entering AP mode...");
        startAPMode();
        beginUDP();

        while (true) {
            if (processUDPPacket()) {
                Serial.println("Configuration received, restarting...");
                delay(1000);
                ESP.restart();
            }
            delay(100);
        }
    } else {
        // Load stored credentials
        strcpy(ssid1, storedSSID.c_str());
        strcpy(password1, storedPassword.c_str());
        strcpy(bossName, storedBossName.c_str());
        strcpy(bossID, storedBossID.c_str());
        
     //   if (mqtt_port == 0) mqtt_port = 1883; // Default MQTT port

        connectToWiFi(ssid1, password1);
        
        // Always maintain AP mode for configuration
        startAPMode();
        beginUDP();
    }
}

void WiFiManager::beginUDP() {
    udp.begin(UDP_PORT);
    Serial.printf("UDP server started on port %d\n", UDP_PORT);
}

bool WiFiManager::processUDPPacket() {
    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
        char packetBuffer[256];
        int len = udp.read(packetBuffer, 255);
        if (len > 0) packetBuffer[len] = '\0';

        Serial.print("Received UDP packet: ");
        Serial.println(packetBuffer);

        // Parse configuration data: ssid//password//bossName//bossID
        const char* userData[] = {"ssid", "password", "bossName", "bossID"};
        char *token = strtok(packetBuffer, "//");
        
        for (int i = 0; i < 4 && token != NULL; i++) {
            storageManager.writeToNVS(userData[i], token);
            token = strtok(NULL, "//");
        }

        sendUDPResponse("Configuration saved successfully");
        return true;
    }
    return false;
}

void WiFiManager::sendUDPResponse(const char* message) {
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.print(message);
    udp.endPacket();
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::isInConfigMode() {
    return isConfigMode;
}

IPAddress WiFiManager::getLocalIP() {
    return WiFi.localIP();
}

IPAddress WiFiManager::getAPIP() {
    return WiFi.softAPIP();
}
