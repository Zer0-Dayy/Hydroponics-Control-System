"""
Simulates a device publishing sensor data to the MQTT broker.
Used for testing the pipeline without physical hardware.
Device name and payload are read from variables.json -> mqttDeviceSimulator.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker      = config["mqttSouthboundBroker"]
port        = config["mqttSouthboundPort"]
device_name = config["mqttDeviceSimulator"]["deviceName"]
payload     = config["mqttDeviceSimulator"]["payload"]
topic       = f"incoming/data/{device_name}/StreamRecord"

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps(payload))
print(f"Published simulated data for {device_name} on {topic}")
client.disconnect()
