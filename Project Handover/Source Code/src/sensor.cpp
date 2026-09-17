#include "sensor.h"

extern const char*  mqtt_topic;
extern PubSubClient client;

Sensor::Sensor(const std::array<int, 4>& gpios, const String& device_name)
    : _gpios(gpios), _device_name(device_name), _lastUpdateMillis(0) {}

String Sensor::getName() { return _device_name; }

std::array<int, 4> Sensor::getGpios() { return _gpios; }

void Sensor::setGpios(const std::array<int, 4>& gpios) { _gpios = gpios; }

void Sensor::publishSingleValue(float value, const char* type) {
    StaticJsonDocument<128> doc;
    doc[type] = value;
    String payload;
    serializeJson(doc, payload);
    String topic = String(mqtt_topic) + "/" + _device_name + "/" + type;
    client.publish(topic.c_str(), payload.c_str());
}

void Sensor::publishSingleValue(int value, const char* type) {
    StaticJsonDocument<128> doc;
    doc[type] = value;
    String payload;
    serializeJson(doc, payload);
    String topic = String(mqtt_topic) + "/" + _device_name + "/" + type;
    client.publish(topic.c_str(), payload.c_str());
}
