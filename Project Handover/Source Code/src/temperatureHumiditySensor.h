#ifndef TEMPERATUREHUMIDITYSENSOR_H
#define TEMPERATUREHUMIDITYSENSOR_H

#include "sensor.h"
#include <DHT.h>

// DHT21 temperature and humidity sensor.
// Reads and publishes every 5 minutes.
class TemperatureHumiditySensor : public Sensor {
public:
    TemperatureHumiditySensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup()  override;
    void loop()   override;
    void updateDevice(const JsonObject& updateInfo) override;
    String getData() override;

private:
    DHT   _dht;
    float _temperature;
    float _humidity;

    void publishToMQTT();
};

#endif
