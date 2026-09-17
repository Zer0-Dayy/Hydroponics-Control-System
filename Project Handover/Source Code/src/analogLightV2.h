#ifndef ANALOGLIGHTV2_H
#define ANALOGLIGHTV2_H

#include "sensor.h"

// Analog light intensity sensor.
// Publishes raw ADC reading every ~1.7 minutes.
// TODO: calibrate raw ADC to lux using the sensor's datasheet.
class AnalogLightV2Sensor : public Sensor {
public:
    AnalogLightV2Sensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup() override;
    void loop()  override;

private:
    int _analogPin;
    int _intensity;

    void publishToMQTT();
};

#endif
