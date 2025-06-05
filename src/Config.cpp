#include "Config.h"

// Cấu hình WiFi AP
const char* AP_SSID = "ESP32_AP";
const char* AP_PASSWORD = "12345678";

// Cấu hình mạng
const IPAddress AP_IP(192, 168, 1, 8);
const IPAddress AP_GATEWAY(192, 168, 1, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);
const unsigned int UDP_PORT = 1234;

// Chân điều khiển relay
const int RELAY_PINS[MAX_PIN] = {26, 33, 32, 14};

// Biến toàn cục
char MACaddress[MAX_PIN][18] = {};
char tokenAddress[MAX_PIN][80] = {};

char ssid1[50] = "";
char password1[50] = "";
char bossName[50] = "";
char bossID[50] = "";
int count_relayOn = 0;