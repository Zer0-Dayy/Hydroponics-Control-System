#include "analogLightV2.h"

AnalogLightV2Sensor::AnalogLightV2Sensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _analogPin(gpios[0]), _intensity(0) {
    _lastUpdateMillis = millis();
}

void AnalogLightV2Sensor::setup() {
    // Analog input — no pinMode needed for ADC pins on ESP32
}

void AnalogLightV2Sensor::loop() {
    if (millis() - _lastUpdateMillis < 5000) return;
    _lastUpdateMillis = millis();

    _intensity = analogRead(_analogPin);
    publishToMQTT();
}

void AnalogLightV2Sensor::publishToMQTT() {
    publishSingleValue(_intensity, "intensity");
}
