#ifndef CONNECT_TO_WIFI_H
#define CONNECT_TO_WIFI_H

#include <Arduino.h>
#include <WiFi.h>

// Blocks until WiFi is connected, then returns.
// Restarts the ESP32 after 30 failed seconds.
void connectToWiFi(const char* ssid, const char* password);

#endif
