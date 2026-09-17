#include "limitSwitch.h"

extern const char*  mqtt_topic;
extern PubSubClient client;

LimitSwitch::LimitSwitch(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _button(gpios[0]), _touched(false), _lastPublishMillis(0) {}

void LimitSwitch::setup() {
    _button.setDebounceTime(50);
}

String LimitSwitch::getData() {
    return _touched ? "true" : "false";
}

void LimitSwitch::loop() {
    _button.loop();
    _touched = (_button.getState() == LOW);  // LOW = pressed (active-low)

    if (millis() - _lastPublishMillis >= 1000) {
        publishToMQTT();
        _lastPublishMillis = millis();
    }
}

void LimitSwitch::publishToMQTT() {
    StaticJsonDocument<64> doc;
    doc["touched"] = _touched;
    String payload;
    serializeJson(doc, payload);
    String topic = String(mqtt_topic) + "/" + _device_name + "/touched";
    client.publish(topic.c_str(), payload.c_str());
}
