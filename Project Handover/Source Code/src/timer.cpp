#include "timer.h"

extern const char*  mqtt_topic;
extern PubSubClient client;

Timer::Timer(const std::array<int, 4>& gpios, const String& device_name)
    : _gpios(gpios), _device_name(device_name),
      _timeClient(_ntpUDP, "pool.ntp.org", UTC_OFFSET_SECONDS),
      _lastUpdateMillis(0) {}

void Timer::setup() {
    _timeClient.begin();
}

void Timer::loop() {
    if (millis() - _lastUpdateMillis < 200000) return;
    _lastUpdateMillis = millis();

    _timeClient.update();
    Serial.println(_device_name + " time: " + _timeClient.getFormattedTime());
    publishToMQTT();
}

String Timer::getName()  { return _device_name; }
std::array<int, 4> Timer::getGpios() { return _gpios; }

void Timer::publishToMQTT() {
    publishSingleValue(_timeClient.getFormattedTime(), "Time");
}

void Timer::publishSingleValue(const String& value, const char* type) {
    StaticJsonDocument<128> doc;
    doc[type] = value;
    String payload;
    serializeJson(doc, payload);
    String topic = String(mqtt_topic) + "/" + _device_name + "/" + type;
    client.publish(topic.c_str(), payload.c_str());
}
