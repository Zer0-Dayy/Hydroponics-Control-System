#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// Abstract base class for all devices (sensors and actuators).
// Every device type must implement at minimum: setup(), loop(), getName(), getGpios().
class Device {
public:
    virtual ~Device() {}
    virtual void setup() {}
    virtual void loop()  {}
    virtual String getName()   { return ""; }
    virtual String getStatus() { return ""; }
    virtual String getData()   { return ""; }
    virtual String getTopic()  { return ""; }
    virtual std::array<int, 4> getGpios()                          { return {-1, -1, -1, -1}; }
    virtual void setGpios(const std::array<int, 4>& gpios)         {}
    virtual void updateDevice(const JsonObject& updateInfo)         {}
    virtual void handleMessage(byte* payload, unsigned int length)  {}
};

#endif
