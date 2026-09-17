#ifndef LIMITSWITCH_H
#define LIMITSWITCH_H

#include "sensor.h"
#include <ezButton.h>

// Debounced limit switch / push button.
// Publishes touched state every second (non-blocking).
class LimitSwitch : public Sensor {
public:
    LimitSwitch(const std::array<int, 4>& gpios, const String& device_name);

    void setup()  override;
    void loop()   override;
    String getData() override;

private:
    ezButton      _button;
    bool          _touched;
    unsigned long _lastPublishMillis;

    void publishToMQTT();
};

#endif
