#ifndef TDSSENSOR_H
#define TDSSENSOR_H

#include "sensor.h"

#define VREF   3.3f   // ESP32 ADC reference voltage
#define SCOUNT 30     // number of samples in the median filter buffer

// TDS (Total Dissolved Solids) sensor with median filtering.
// Samples ADC every 8 seconds into a ring buffer.
// Every 4 minutes: applies median filter, temperature compensation, and publishes in ppm.
// Note: _temperature is fixed at 25°C. Wire in a WaterTemp sensor and share its
// reading here for accurate compensation.
class TdsSensor : public Sensor {
public:
    TdsSensor(const std::array<int, 4>& gpios, const String& device_name);

    void setup() override;
    void loop()  override;

private:
    int           _analogPin;
    int           _tdsValue;
    int           _analogBuffer[SCOUNT];
    int           _analogBufferTemp[SCOUNT];
    int           _analogBufferIndex;
    float         _averageVoltage;
    float         _temperature;      // for compensation — fixed at 25°C
    unsigned long _lastPublishMillis;

    int  getMedianNum(int bArray[], int iFilterLen);
    void publishToMQTT();
};

#endif
