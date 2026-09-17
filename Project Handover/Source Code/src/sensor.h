#ifndef SENSOR_H
#define SENSOR_H

#include "device.h"

// Intermediate base class for all read-only sensors.
// Provides shared GPIO storage, timing, and MQTT publish helpers.
class Sensor : public Device {
public:
    Sensor(const std::array<int, 4>& gpios, const String& device_name);

    String getName() override;
    std::array<int, 4> getGpios() override;
    void setGpios(const std::array<int, 4>& gpios) override;
    void updateDevice(const JsonObject& updateInfo) override {}

    void publishSingleValue(float value, const char* type);
    void publishSingleValue(int   value, const char* type);

protected:
    std::array<int, 4> _gpios;
    String             _device_name;
    unsigned long      _lastUpdateMillis;
};

#endif
