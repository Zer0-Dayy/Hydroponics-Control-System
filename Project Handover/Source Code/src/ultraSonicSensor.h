#ifndef ULTRASONICSENSOR_H
#define ULTRASONICSENSOR_H

#include "sensor.h"

// HC-SR04 ultrasonic distance sensor.
// gpios[0] = TRIG, gpios[1] = ECHO.
// Measures and publishes distance in cm every ~3.3 minutes.
class UltraSonicSensor : public Sensor {
public:
    UltraSonicSensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup() override;
    void loop()  override;

private:
    int   _trigPin;
    int   _echoPin;
    float _distance;

    static constexpr float SOUND_SPEED_CM_US = 0.034f;  // cm per microsecond

    void publishToMQTT();
};

#endif
