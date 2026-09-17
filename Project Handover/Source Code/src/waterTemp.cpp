#include "waterTemp.h"

WaterTemp::WaterTemp(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _wire(gpios[0]), _sensors(&_wire), _waterTemperature(0) {}

void WaterTemp::setup() {
    _sensors.begin();
}

void WaterTemp::loop() {
    if (millis() - _lastUpdateMillis < 100000) return;
    _lastUpdateMillis = millis();

    _sensors.requestTemperatures();
    _waterTemperature = _sensors.getTempCByIndex(0);

    publishToMQTT();
}

void WaterTemp::publishToMQTT() {
    publishSingleValue(_waterTemperature, "water temperature");
}
