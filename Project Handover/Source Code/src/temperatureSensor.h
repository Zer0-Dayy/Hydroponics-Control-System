#ifndef TEMPERATURESENSOR_H
#define TEMPERATURESENSOR_H

#include "sensor.h"

// Generic analog temperature sensor.
// Converts ESP32 ADC (12-bit, 3.3V) reading to voltage and publishes every 5 minutes.
// Note: replace getData() formula with the specific sensor's voltage-to-temperature curve.
class TemperatureSensor : public Sensor {
public:
    TemperatureSensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup()  override;
    void loop()   override;
    String getData()   override;
    String getStatus() override;

private:
    int _gpio;
};

#endif
