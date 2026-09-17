"""
Deletes all devices that share a given profile name from EdgeX via MQTT.
The profile name is read from variables.json -> deleteDevicesByProfile -> deviceProfileName.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker       = config["mqttBroker"]
port         = config["mqttPort"]
topic        = config["mqttTopicDeleteDevicesByProfile"]
profile_name = config["deleteDevicesByProfile"]["deviceProfileName"]

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps({"profileName": profile_name}))
print(f"Deleted all devices with profile: {profile_name}")
client.disconnect()
