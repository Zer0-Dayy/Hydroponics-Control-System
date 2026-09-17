#ifndef TIMER_H
#define TIMER_H

#include "device.h"
#include <Arduino.h>
#include <WiFiUDP.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// NTP-synced real-time clock.
// Publishes the current time (HH:MM:SS) every ~3.3 minutes.
// UTC offset is set to GMT+1 (3600 seconds). Update as needed.
class Timer : public Device {
public:
    Timer(const std::array<int, 4>& gpios, const String& device_name);

    void setup()  override;
    void loop()   override;
    void updateDevice(const JsonObject& updateInfo) override {}
    String getName() override;
    std::array<int, 4> getGpios() override;

private:
    std::array<int, 4> _gpios;
    String             _device_name;
    WiFiUDP            _ntpUDP;
    NTPClient          _timeClient;
    unsigned long      _lastUpdateMillis;

    static constexpr long UTC_OFFSET_SECONDS = 3600;  // GMT+1

    void publishToMQTT();
    void publishSingleValue(const String& value, const char* type);
};

#endif
