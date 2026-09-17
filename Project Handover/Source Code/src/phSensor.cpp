#include "phSensor.h"

PhSensor::PhSensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _analogPin(gpios[0]), _phValue(0) {
    _lastUpdateMillis = millis();
}

void PhSensor::setup() {
    // Analog input — no pinMode needed for ADC pins on ESP32
}

void PhSensor::loop() {
    if (millis() - _lastUpdateMillis < 5000) return;
    _lastUpdateMillis = millis();

    float voltage = analogRead(_analogPin) * (3.3f / 4095.0f);
    _phValue = 7.2f - (2.622f - voltage) * M;

    publishToMQTT();
}

void PhSensor::publishToMQTT() {
    publishSingleValue(_phValue, "Ph value");
}
