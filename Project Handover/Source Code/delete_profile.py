"""
Deletes a device profile from EdgeX via MQTT.
The profile name is read from variables.json -> deleteProfile -> deviceProfileName.
Note: all devices using this profile must be deleted first.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker       = config["mqttBroker"]
port         = config["mqttPort"]
topic        = config["mqttTopicDeleteProfile"]
profile_name = config["deleteProfile"]["deviceProfileName"]

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps({"profileName": profile_name}))
print(f"Deleted profile: {profile_name}")
client.disconnect()
