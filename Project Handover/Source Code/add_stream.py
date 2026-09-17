"""
Creates an eKuiper data stream by publishing an SQL definition to the gateway via MQTT.
The stream must exist before rules that reference it can be created.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker  = config["mqttBroker"]
port    = config["mqttPort"]
topic   = config["mqttTopicNewStream"]
sql     = config["addStream"]["sql"]

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps({"sql": sql}))
print(f"Created stream with SQL: {sql}")
client.disconnect()
