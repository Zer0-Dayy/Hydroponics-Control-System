#include "relayActuator.h"

extern const char*  mqtt_topic;
extern PubSubClient client;

RelayActuator::RelayActuator(const std::array<int, 4>& gpios, const String& device_name)
    : _gpios(gpios), _device_name(device_name), _lastPublishMillis(0) {
    _gpio  = _gpios[0];
    _topic = "Command/" + device_name;
}

void RelayActuator::setup() {
    pinMode(_gpio, OUTPUT);
    digitalWrite(_gpio, HIGH);  // relay off (active-low)
}

void RelayActuator::loop() {
    client.loop();
    if (millis() - _lastPublishMillis >= 120000) {
        publishStatus();
        _lastPublishMillis = millis();
    }
}

String RelayActuator::getName()  { return _device_name; }
String RelayActuator::getTopic() { return _topic; }
std::array<int, 4> RelayActuator::getGpios() { return _gpios; }

void RelayActuator::handleMessage(byte* payload, unsigned int length) {
    StaticJsonDocument<128> doc;
    if (deserializeJson(doc, payload, length) != DeserializationError::Ok) {
        Serial.println("RelayActuator: invalid JSON");
        return;
    }
    if (!doc.containsKey("Working")) {
        Serial.println("RelayActuator: missing 'Working' key");
        return;
    }
    bool cmd = doc["Working"];
    cmd ? activate() : deactivate();
    Serial.println(_device_name + " relay " + (cmd ? "ON" : "OFF") + " (GPIO " + _gpio + ")");
}

void RelayActuator::activate()  { digitalWrite(_gpio, LOW);  }
void RelayActuator::deactivate(){ digitalWrite(_gpio, HIGH); }
bool RelayActuator::isActive()  { return !digitalRead(_gpio); }

void RelayActuator::publishStatus() {
    publishSingleValue(isActive(), "Status");
}

void RelayActuator::publishSingleValue(bool value, const char* type) {
    StaticJsonDocument<128> doc;
    doc[type] = value;
    String payload;
    serializeJson(doc, payload);
    String topic = String(mqtt_topic) + "/" + _device_name + "/" + type;
    client.publish(topic.c_str(), payload.c_str());
}
