#include "deviceFactory.h"

Device* DeviceFactory::createDevice(const String& profileName,
                                    const std::array<int, 4>& gpios,
                                    const String& device_name) {
    Serial.println("Creating device — profile: " + profileName + ", name: " + device_name);

    if      (profileName == "Capacitive_Soil_Moisture_Sensor_v1-2") return new MoistureSensor(gpios, device_name);
    else if (profileName == "Relay_Actuator")                        return new RelayActuator(gpios, device_name);
    else if (profileName == "Temperature_Humidity_DHT21")            return new TemperatureHumiditySensor(gpios, device_name);
    else if (profileName == "Limit_Switch")                          return new LimitSwitch(gpios, device_name);
    else if (profileName == "Temperature_Sensor")                    return new TemperatureSensor(gpios, device_name);
    else if (profileName == "Ultra_Sonic_Sensor")                    return new UltraSonicSensor(gpios, device_name);
    else if (profileName == "MQ_7")                                  return new Mq7Sensor(gpios, device_name);
    else if (profileName == "Analog_Light")                          return new AnalogLightV2Sensor(gpios, device_name);
    else if (profileName == "Ph_sensor")                             return new PhSensor(gpios, device_name);
    else if (profileName == "TDS_sensor")                            return new TdsSensor(gpios, device_name);
    else if (profileName == "WaterTemp_sensor")                      return new WaterTemp(gpios, device_name);
    else if (profileName == "Timer")                                  return new Timer(gpios, device_name);

    Serial.println("Unknown profile: " + profileName);
    return nullptr;
}
