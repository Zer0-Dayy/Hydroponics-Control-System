#ifndef PHSENSOR_H
#define PHSENSOR_H

#include "sensor.h"

// pH sensor (4502C analog module).
// Formula: pH = 7.2 - (2.622 - Vout) * M
// M = -6.0994 — calibrate per unit using pH 4 and pH 7 buffer solutions.
// Reads and publishes every ~6.7 minutes.
class PhSensor : public Sensor {
public:
    PhSensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup() override;
    void loop()  override;

private:
    int   _analogPin;
    float _phValue;

    // Calibration slope: m = (pH7 - pH4) / (V_pH7 - V_pH4)
    // Measure your own voltages using the ph_calibrate sketch and update this value.
    static constexpr float M = -6.0994f;

    void publishToMQTT();
};

#endif
