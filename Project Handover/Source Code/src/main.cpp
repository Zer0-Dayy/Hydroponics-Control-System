#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ezButton.h>
#include "connectToWifi.h"
#include "device.h"
#include "sensor.h"
#include "relayActuator.h"
#include "moistureSensor.h"
#include "temperatureHumiditySensor.h"
#include "limitSwitch.h"
#include "deviceFactory.h"
#include "deviceManager.h"

// ─── Network configuration ────────────────────────────────────────────────────
const char* ssid        = "LIJO-SMU";
const char* password    = "3@7984A5B";
const char* mqtt_server = "192.168.10.102";
const int   mqtt_port   = 31883;
const char* mqtt_topic  = "incoming/data";
const char* esp_name    = "ESP1";
const char* devices_url = "http://192.168.10.102:30300/configs";
// ─────────────────────────────────────────────────────────────────────────────

WiFiClient    espClient;
PubSubClient  client(espClient);
HTTPClient    http;
DeviceManager deviceManager;

// Fetches the device list for this ESP from the config server and
// applies any additions, removals, or updates to the DeviceManager.
void fetchDevicesAndUpdate(DeviceManager& dm) {
    String url = String(devices_url) + "/" + String(esp_name);
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String json = http.getString();
        Serial.println(json);
        dm.updateDevices(json);
    } else {
        Serial.println("Failed to fetch device config. HTTP: " + String(httpCode));
    }

    http.end();
}

// Routes incoming MQTT commands to the correct device by parsing the topic.
// Expected topic format: Command/<device_name>/...
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String topicStr   = String(topic);
    int firstSlash    = topicStr.indexOf('/');
    int secondSlash   = topicStr.indexOf('/', firstSlash + 1);
    String deviceName = topicStr.substring(firstSlash + 1, secondSlash);

    for (auto& kv : deviceManager.getDevices()) {
        if (deviceName == kv.second->getName()) {
            kv.second->handleMessage(payload, length);
            Serial.println("Command dispatched to: " + deviceName);
            return;
        }
    }

    Serial.println("No device matched command topic: " + topicStr);
}

// Reconnects to MQTT broker. Restarts the ESP32 after 5 failed attempts.
void reconnect() {
    client.setServer(mqtt_server, mqtt_port);
    int attempts = 0;

    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
        if (client.connect(esp_name)) {
            Serial.println("MQTT connected.");
            client.subscribe("Command/#");
        } else {
            Serial.println("MQTT failed (rc=" + String(client.state()) + "). Retrying in 5s...");
            delay(5000);
            if (++attempts > 5) {
                Serial.println("MQTT unreachable. Restarting...");
                ESP.restart();
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    connectToWiFi(ssid, password);

    client.setServer(mqtt_server, mqtt_port);
    client.setKeepAlive(60);
    client.setCallback(mqttCallback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    // Run each device's non-blocking loop
    for (auto const& [name, device] : deviceManager.getDevices()) {
        device->loop();
    }

    // Sync device list from server every 30 seconds
    static unsigned long lastFetch = 0;
    if (millis() - lastFetch >= 30000) {
        fetchDevicesAndUpdate(deviceManager);
        deviceManager.printDevices();
        lastFetch = millis();
    }
}
