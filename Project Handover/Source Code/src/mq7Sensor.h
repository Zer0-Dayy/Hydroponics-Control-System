#ifndef MQ7SENSOR_H
#define MQ7SENSOR_H

#include "sensor.h"

// MQ-7 carbon monoxide sensor.
// gpios[0] = DOUT (digital threshold), gpios[1] = AOUT (analog concentration).
// Reads and publishes every 10 minutes.
class Mq7Sensor : public Sensor {
public:
    Mq7Sensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup() override;
    void loop()  override;

private:
    int _digitalPin;
    int _analogPin;
    int _value;   // raw analog reading
    int _limit;   // digital threshold output (0 or 1)

    void publishToMQTT();
};

#endif
