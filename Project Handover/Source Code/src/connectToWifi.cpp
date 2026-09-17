#include "connectToWifi.h"

void connectToWiFi(const char* ssid, const char* password) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
        if (++attempts > 30) {
            Serial.println("\nWiFi connection failed. Restarting...");
            ESP.restart();
        }
    }

    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
}
