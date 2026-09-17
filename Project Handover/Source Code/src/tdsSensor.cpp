#include "tdsSensor.h"

TdsSensor::TdsSensor(const std::array<int, 4>& gpios, const String& device_name)
    : Sensor(gpios, device_name),
      _analogPin(gpios[0]),
      _tdsValue(0),
      _analogBufferIndex(0),
      _averageVoltage(0),
      _temperature(25.0f),
      _lastPublishMillis(0) {
    memset(_analogBuffer, 0, sizeof(_analogBuffer));
    memset(_analogBufferTemp, 0, sizeof(_analogBufferTemp));
}

void TdsSensor::setup() {
    pinMode(_analogPin, INPUT);
}

// Insertion-sort median to avoid a VLA on the stack
int TdsSensor::getMedianNum(int bArray[], int iFilterLen) {
    int bTab[SCOUNT];
    for (int i = 0; i < iFilterLen; i++) bTab[i] = bArray[i];

    for (int j = 0; j < iFilterLen - 1; j++) {
        for (int i = 0; i < iFilterLen - j - 1; i++) {
            if (bTab[i] > bTab[i + 1]) {
                int tmp = bTab[i]; bTab[i] = bTab[i + 1]; bTab[i + 1] = tmp;
            }
        }
    }
    return (iFilterLen & 1) ? bTab[(iFilterLen - 1) / 2]
                            : (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
}

void TdsSensor::loop() {
    // Sample ADC into ring buffer every 8 seconds
    if (millis() - _lastUpdateMillis >= 8000) {
        _lastUpdateMillis = millis();
        _analogBuffer[_analogBufferIndex] = analogRead(_analogPin);
        _analogBufferIndex = (_analogBufferIndex + 1) % SCOUNT;
    }

    // Calculate and publish every 4 minutes
    if (millis() - _lastPublishMillis >= 5000) {
        _lastPublishMillis = millis();

        // Copy buffer for sorting (preserves ring buffer order)
        for (int i = 0; i < SCOUNT; i++) {
            _analogBufferTemp[i] = _analogBuffer[i];
        }

        // Median filter → voltage → temperature compensation → TDS
        _averageVoltage = getMedianNum(_analogBufferTemp, SCOUNT) * VREF / 4096.0f;
        float compCoeff    = 1.0f + 0.02f * (_temperature - 25.0f);
        float compVoltage  = _averageVoltage / compCoeff;
        _tdsValue = (int)((133.42f * compVoltage * compVoltage * compVoltage
                         - 255.86f * compVoltage * compVoltage
                         + 857.39f * compVoltage) * 0.5f);

        publishToMQTT();
    }
}

void TdsSensor::publishToMQTT() {
    publishSingleValue(_tdsValue, "TDS value");
}
