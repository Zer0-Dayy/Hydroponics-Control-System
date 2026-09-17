#include "moistureSensor.h"

extern const char*  mqtt_topic;
extern PubSubClient client;

MoistureSensor::MoistureSensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _gpio(gpios[0]), _lastPublishMillis(0) {}

void MoistureSensor::setup() {
    pinMode(_gpio, INPUT);
}

String MoistureSensor::getData() {
    int raw = analogRead(_gpio);
    int pct = map(raw, AIR_VALUE, WATER_VALUE, 0, 100);
    pct = constrain(pct, 0, 100);
    return String(pct);
}

String MoistureSensor::getStatus() { return "OK"; }

void MoistureSensor::loop() {
    if (millis() - _lastPublishMillis < 600000) return;
    _lastPublishMillis = millis();
    publishToMQTT(getData().toInt());
}

void MoistureSensor::publishToMQTT(int moisture_pct) {
    StaticJsonDocument<64> doc;
    doc["soil_moisture"] = moisture_pct;
    String payload;
    serializeJson(doc, payload);
    String topic = String(mqtt_topic) + "/" + _device_name + "/soil_moisture";
    client.publish(topic.c_str(), payload.c_str());
}
