#ifndef RELAYACTUATOR_H
#define RELAYACTUATOR_H

#include "device.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Controls a relay module via a single GPIO.
// Listens for {"Working": true/false} MQTT commands.
// Publishes relay state every 2 minutes.
class RelayActuator : public Device {
public:
    RelayActuator(const std::array<int, 4>& gpios, const String& device_name);

    void setup()   override;
    void loop()    override;
    String getName() override;
    String getTopic() override;
    std::array<int, 4> getGpios() override;
    void updateDevice(const JsonObject& updateInfo) override {}
    void handleMessage(byte* payload, unsigned int length) override;

private:
    int                _gpio;
    std::array<int, 4> _gpios;
    String             _device_name;
    String             _topic;
    unsigned long      _lastPublishMillis;

    void activate();
    void deactivate();
    bool isActive();
    void publishStatus();
    void publishSingleValue(bool value, const char* type);
};

#endif
