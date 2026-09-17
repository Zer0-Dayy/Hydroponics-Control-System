#include "temperatureSensor.h"

TemperatureSensor::TemperatureSensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _gpio(gpios[0]) {}

void TemperatureSensor::setup() {
    pinMode(_gpio, INPUT);
}

// Converts 12-bit ESP32 ADC reading to voltage (3.3V reference).
// Replace with the sensor-specific voltage-to-temperature formula as needed.
String TemperatureSensor::getData() {
    float voltage = analogRead(_gpio) * (3.3f / 4095.0f);
    return String(voltage, 3);
}

String TemperatureSensor::getStatus() { return "OK"; }

void TemperatureSensor::loop() {
    if (millis() - _lastUpdateMillis < 300000) return;
    _lastUpdateMillis = millis();
    float voltage = analogRead(_gpio) * (3.3f / 4095.0f);
    publishSingleValue(voltage, "temperature");
}
