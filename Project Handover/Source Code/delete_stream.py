"""
Deletes an eKuiper data stream via MQTT.
The stream name is read from variables.json -> deleteStream -> streamName.
Note: rules that reference this stream must be deleted first.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker      = config["mqttBroker"]
port        = config["mqttPort"]
topic       = config["mqttTopicDeleteStream"]
stream_name = config["deleteStream"]["streamName"]

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps({"streamName": stream_name}))
print(f"Deleted stream: {stream_name}")
client.disconnect()
