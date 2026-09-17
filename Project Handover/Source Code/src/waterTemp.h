#ifndef WATERTEMP_H
#define WATERTEMP_H

#include "sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 waterproof OneWire temperature sensor.
// Reads and publishes water temperature every ~1.7 minutes.
class WaterTemp : public Sensor {
public:
    WaterTemp(const std::array<int, 4>& gpios, const String& device_name);

    void setup() override;
    void loop()  override;

private:
    OneWire           _wire;
    DallasTemperature _sensors;
    float             _waterTemperature;

    void publishToMQTT();
};

#endif
