"""
Deletes an eKuiper automation rule via MQTT.
The rule name is read from variables.json -> deleteRule -> ruleName.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker    = config["mqttBroker"]
port      = config["mqttPort"]
topic     = config["mqttTopicDeleteRule"]
rule_name = config["deleteRule"]["ruleName"]

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps({"ruleName": rule_name}))
print(f"Deleted rule: {rule_name}")
client.disconnect()
