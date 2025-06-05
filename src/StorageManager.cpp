#include "StorageManager.h"
#include "Config.h"

StorageManager storageManager;

StorageManager::StorageManager() {
}

StorageManager::~StorageManager() {
    preferences.end();
}

void StorageManager::begin() {
    EEPROM.begin(EEPROM_SIZE);
}

void StorageManager::clearEEPROM() {
    EEPROM.begin(EEPROM_SIZE);
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, 0xFF);
    }
    EEPROM.commit();
    Serial.println("EEPROM cleared!");
    digitalWrite(19,HIGH);
    delay(500);
    digitalWrite(19,LOW);
    delay(500);
    digitalWrite(19,HIGH);
    delay(500);
    digitalWrite(19,LOW);
    delay(500);
}

void StorageManager::writeMACsToPROM(char MACaddress[][18],char tokenAddress[][80], int count) {
    EEPROM.begin(EEPROM_SIZE);
    clearEEPROM();

    int addr = 0;
    for (int i = 0; i < count; i++) {
        String entry = String(i) + "=" + String(MACaddress[i])+ "?" +String(tokenAddress[i]) + "/";
        for (int j = 0; j < entry.length(); j++) {
            EEPROM.write(addr++, entry[j]);
        }
    }

    EEPROM.write(addr, '\0');
    EEPROM.commit();
    Serial.println("MAC addresses and token saved to EEPROM!");
    digitalWrite(19,HIGH);
    delay(500);
    digitalWrite(19,LOW);
    delay(500);
    digitalWrite(19,HIGH);
    delay(500);
    digitalWrite(19,LOW);
    delay(500);


}

int StorageManager::readMACsFromPROM(char MACaddress[][18], char tokenAddress[][80]) {
    EEPROM.begin(EEPROM_SIZE);

    char buffer[EEPROM_SIZE];
    for (int i = 0; i < EEPROM_SIZE; i++) {
        buffer[i] = EEPROM.read(i);
        if (buffer[i] == '\0') {
            buffer[i] = '\0'; // đảm bảo kết thúc chuỗi
            break;
        }
    }

    Serial.println("EEPROM Data:");
    Serial.println(buffer);

    int count = 0;
    char *token = strtok(buffer, "/");  // tách từng cặp "index=MAC:TOKEN"
    while (token != NULL) {
        int index;
        char mac[18];
        char t[50];

        // dùng sscanf để parse đúng format "index=MAC?TOKEN"
        if (sscanf(token, "%d=%17[^?]?%49s", &index, mac, t) == 3) {
            if (index >= 0 && index < MAX_PIN) {
                strcpy(MACaddress[index], mac);
                strcpy(tokenAddress[index], t);
                count++;
            }
        }

        token = strtok(NULL, "/");
    }

    Serial.printf("Loaded %d devices from EEPROM\n", count);
     digitalWrite(19,HIGH);
    delay(500);
    digitalWrite(19,LOW);
    delay(500);
    digitalWrite(19,HIGH);
    delay(500);
    digitalWrite(19,LOW);
    delay(500);
    return count;
}




void StorageManager::writeToNVS(const char* key, const char* value) {
    preferences.begin("storage", false);
    preferences.putString(key, value);
    preferences.end();
    Serial.printf("NVS: Saved %s = %s\n", key, value);
    digitalWrite(18,HIGH);
    delay(500);
    digitalWrite(18,LOW);
    delay(500);
    digitalWrite(18,HIGH);
    delay(500);
    digitalWrite(18,LOW);
    delay(500);
}

String StorageManager::readFromNVS(const char* key) {
    preferences.begin("storage", true);
    String value = preferences.getString(key, "");
    preferences.end();
    return value;
}

void StorageManager::clearWiFiCredentials() {
    Serial.println("Clearing WiFi credentials...");
    preferences.begin("storage", false);
    preferences.remove("ssid");
    preferences.remove("password");
    preferences.remove("mqtt_token");
    preferences.remove("mqttName");
    preferences.remove("bossID");
    preferences.end();
    Serial.println("WiFi credentials cleared!");
    digitalWrite(18,HIGH);
    delay(500);
    digitalWrite(18,LOW);
    delay(500);
    digitalWrite(18,HIGH);
    delay(500);
    digitalWrite(18,LOW);
    delay(500);
}