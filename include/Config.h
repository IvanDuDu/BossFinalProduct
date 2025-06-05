#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Cấu hình chung
#define MAX_PIN 4
#define EEPROM_SIZE 512
#define CLEAR_BUTTON_PIN 0

// Cấu hình WiFi AP
extern const char* AP_SSID;
extern const char* AP_PASSWORD;

// Cấu hình mạng
extern const IPAddress AP_IP;
extern const IPAddress AP_GATEWAY;
extern const IPAddress AP_SUBNET;
extern const unsigned int UDP_PORT;

// Chân điều khiển relay
extern const int RELAY_PINS[MAX_PIN];

// Biến toàn cục
extern char MACaddress[MAX_PIN][18];
extern char tokenAddress[MAX_PIN][80];
extern char ssid1[50];
extern char password1[50];
const char mqtt_server[100]= "mqtt.thingsboard.cloud" ;
extern char bossName[50];
extern char bossID[50];
const int mqtt_port = 1883;
extern int count_relayOn;
const char mqtt_default_token[100]="793XAOhIc36uyZFZIy1K";


//config token là 1 list các token của từng staff, truy cập theo token
// sửa đăng ký vào địa chỉ cố định.
#endif