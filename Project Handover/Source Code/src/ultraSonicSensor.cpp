#include "ultraSonicSensor.h"

UltraSonicSensor::UltraSonicSensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _trigPin(gpios[0]), _echoPin(gpios[1]), _distance(0) {
    _lastUpdateMillis = millis();
}

void UltraSonicSensor::setup() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
}

void UltraSonicSensor::loop() {
    if (millis() - _lastUpdateMillis < 5000) return;
    _lastUpdateMillis = millis();

    // Trigger pulse
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Measure echo duration and convert to distance
    float duration = pulseIn(_echoPin, HIGH);
    _distance = duration * SOUND_SPEED_CM_US / 2.0f;

    publishToMQTT();
}

void UltraSonicSensor::publishToMQTT() {
    publishSingleValue(_distance, "distance");
}
