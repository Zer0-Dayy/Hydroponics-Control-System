#include "temperatureHumiditySensor.h"

extern const char*  mqtt_topic;
extern PubSubClient client;

TemperatureHumiditySensor::TemperatureHumiditySensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name), _dht(gpios[0], DHT22), _temperature(0), _humidity(0) {}

void TemperatureHumiditySensor::setup() {
    _dht.begin();
    pinMode(_gpios[0], INPUT_PULLUP);
}

void TemperatureHumiditySensor::updateDevice(const JsonObject& updateInfo) {
    JsonArray gpioArray = updateInfo["gpios"].as<JsonArray>();
    int newGpio = gpioArray[0];
    if (newGpio != _gpios[0]) {
        _gpios[0] = newGpio;
        _dht = DHT(_gpios[0], DHT22);
        setup();
    }
}

String TemperatureHumiditySensor::getData() {
    StaticJsonDocument<128> doc;
    doc["temperature"] = _dht.readTemperature();
    doc["humidity"]    = _dht.readHumidity();
    String out;
    serializeJson(doc, out);
    return out;
}

void TemperatureHumiditySensor::loop() {
    if (millis() - _lastUpdateMillis < 5000) return;
    _lastUpdateMillis = millis();

    float t = _dht.readTemperature();
    float h = _dht.readHumidity();

    if (isnan(t) || isnan(h)) {
        Serial.println(_device_name + ": DHT read failed");
        return;
    }

    _temperature = t;
    _humidity    = h;
    publishToMQTT();
}

void TemperatureHumiditySensor::publishToMQTT() {
    publishSingleValue(_temperature, "temperature");
    publishSingleValue(_humidity,    "humidity");
}
