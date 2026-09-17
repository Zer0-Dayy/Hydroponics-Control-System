"""
Deletes a single device from EdgeX by name via MQTT.
The device name is read from variables.json -> deleteDevice -> deviceName.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker      = config["mqttBroker"]
port        = config["mqttPort"]
topic       = config["mqttTopicDeleteDevice"]
device_name = config["deleteDevice"]["deviceName"]

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps({"deviceName": device_name}))
print(f"Deleted device: {device_name}")
client.disconnect()
