"""
Creates an eKuiper automation rule by publishing it to the gateway via MQTT.
The rule runs a SQL query on a stream and triggers a REST PUT command on a device
when the condition is met.
"""

import json

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker         = config["mqttBroker"]
port           = config["mqttPort"]
topic          = config["mqttTopicNewRule"]
core_cmd_ip    = config["coreCommandIp"]
rule_name      = config["addRule"]["ruleName"]
sql_query      = config["addRule"]["sqlQuery"]
device_name    = config["addRule"]["deviceName"]
resource_name  = config["addRule"]["resourceName"]
resource_value = config["addRule"]["resourceValue"]

message = {
    "id" : rule_name,
    "sql": sql_query,
    "actions": [
        {
            "rest": {
                "url"          : f"http://{core_cmd_ip}:59882/api/v3/device/name/{device_name}/{resource_name}",
                "method"       : "put",
                "dataTemplate" : json.dumps({resource_name: resource_value}),
                "sendSingle"   : True
            }
        }
    ]
}

client = mqtt.Client()
client.connect(broker, port)
client.publish(topic, json.dumps(message))
print(f"Created rule: {rule_name}")
client.disconnect()
