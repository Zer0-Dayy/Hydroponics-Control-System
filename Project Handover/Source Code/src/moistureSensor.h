#ifndef MOISTURESENSOR_H
#define MOISTURESENSOR_H

#include "sensor.h"

// Capacitive soil moisture sensor v1.2.
// Maps raw ADC (1360–3400) to 0–100% and publishes every 10 minutes.
class MoistureSensor : public Sensor {
public:
    MoistureSensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup()  override;
    void loop()   override;
    String getData()   override;
    String getStatus() override;

private:
    int           _gpio;
    unsigned long _lastPublishMillis;

    static constexpr int AIR_VALUE   = 3400;
    static constexpr int WATER_VALUE = 1360;

    void publishToMQTT(int moisture_pct);
};

#endif
