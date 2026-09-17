"""
Deletes all devices registered in EdgeX via MQTT.
Use with caution — this removes every device in the system.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker = config["mqttBroker"]
port   = config["mqttPort"]
topic  = config["mqttTopicDeleteAllDevices"]

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps({}))
print("Sent delete-all-devices command.")
client.disconnect()
