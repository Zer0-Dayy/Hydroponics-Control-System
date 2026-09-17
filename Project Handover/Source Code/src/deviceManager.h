#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <Arduino.h>
#include <map>
#include "device.h"
#include "deviceFactory.h"

// Owns all active Device instances.
// Handles dynamic add / remove / update from the server JSON config.
class DeviceManager {
public:
    void addDevice(const std::array<int, 4>& gpios, const String& profileName, const String& device_name);
    void removeDevice(const String& device_name);
    void updateDevices(const String& devicesJson);
    std::map<String, Device*>& getDevices();
    void printDevices();

private:
    std::map<String, Device*> _devices;  // keyed by device name
};

#endif
