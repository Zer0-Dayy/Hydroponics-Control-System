#include "mq7Sensor.h"

Mq7Sensor::Mq7Sensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _digitalPin(gpios[0]), _analogPin(gpios[1]), _value(0), _limit(0) {
    _lastUpdateMillis = millis();
}

void Mq7Sensor::setup() {
    pinMode(_digitalPin, INPUT);
}

void Mq7Sensor::loop() {
    if (millis() - _lastUpdateMillis < 5000) return;
    _lastUpdateMillis = millis();

_value = analogRead(_analogPin);
_limit = digitalRead(_digitalPin);

    Serial.println(_device_name + " CO value: " + _value + ", limit: " + _limit);
    publishToMQTT();
}

void Mq7Sensor::publishToMQTT() {
    publishSingleValue(_value, "value");
    publishSingleValue(_limit, "limit");
}
