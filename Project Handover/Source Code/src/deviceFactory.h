#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H

#include "device.h"
#include "moistureSensor.h"
#include "temperatureHumiditySensor.h"
#include "relayActuator.h"
#include "limitSwitch.h"
#include "temperatureSensor.h"
#include "ultraSonicSensor.h"
#include "mq7Sensor.h"
#include "analogLightV2.h"
#include "phSensor.h"
#include "tdsSensor.h"
#include "waterTemp.h"
#include "timer.h"

// Creates the correct Device subclass based on the profile name string
// received from the server config.
class DeviceFactory {
public:
    static Device* createDevice(const String& profileName,
                                const std::array<int, 4>& gpios,
                                const String& device_name);
};

#endif
