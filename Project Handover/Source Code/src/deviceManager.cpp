#include "deviceManager.h"

extern const char* esp_name;

void DeviceManager::addDevice(const std::array<int, 4>& gpios, const String& profileName, const String& device_name) {
    if (_devices.count(device_name)) {
        Serial.println("Device already exists: " + device_name);
        return;
    }
    Device* device = DeviceFactory::createDevice(profileName, gpios, device_name);
    if (device) {
        _devices[device_name] = device;
        device->setup();
    } else {
        Serial.println("Unknown profile: " + profileName);
    }
}

void DeviceManager::removeDevice(const String& device_name) {
    auto it = _devices.find(device_name);
    if (it != _devices.end()) {
        delete it->second;
        _devices.erase(it);
    } else {
        Serial.println("Device not found: " + device_name);
    }
}

void DeviceManager::updateDevices(const String& devicesJson) {
    DynamicJsonDocument doc(8192);
    if (deserializeJson(doc, devicesJson) != DeserializationError::Ok) {
        Serial.println("Failed to parse device config JSON.");
        return;
    }

    // Build a map of devices intended for this ESP from the received JSON
    std::map<String, JsonObject> incoming;
    for (auto kv : doc.as<JsonObject>()) {
        JsonObject info = kv.value().as<JsonObject>();
        if (info["espName"].as<String>() != esp_name) continue;
        incoming[info["name"].as<String>()] = info;
    }

    // Remove devices no longer present in the config
    for (auto it = _devices.begin(); it != _devices.end(); ) {
        if (!incoming.count(it->first)) {
            delete it->second;
            it = _devices.erase(it);
        } else {
            ++it;
        }
    }

    // Update existing devices or add new ones
    for (const auto& kv : incoming) {
        const String&     name = kv.first;
        const JsonObject& info = kv.second;

        if (_devices.count(name)) {
            _devices[name]->updateDevice(info);
        } else {
            JsonArray gpioArray = info["gpios"].as<JsonArray>();
            std::array<int, 4> gpios = {-1, -1, -1, -1};
            unsigned int i = 0;
            for (int pin : gpioArray) {
                if (i < gpios.size()) gpios[i++] = pin;
            }
            addDevice(gpios, info["profileName"].as<String>(), name);
        }
    }
}

std::map<String, Device*>& DeviceManager::getDevices() {
    return _devices;
}

void DeviceManager::printDevices() {
    String output = "Active devices:\n";
    for (const auto& kv : _devices) {
        std::array<int, 4> gpios = kv.second->getGpios();
        String gpioStr;
        for (int pin : gpios) {
            if (pin != -1) {
                if (!gpioStr.isEmpty()) gpioStr += ", ";
                gpioStr += String(pin);
            }
        }
        output += "  " + kv.first + " [GPIOs: " + gpioStr + "]\n";
    }
    Serial.println(output);
}
