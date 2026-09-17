"""
Sends a command to a device through the EdgeX command service via MQTT.
The payload is base64-encoded as required by the EdgeX v3 MQTT protocol.
"""

import base64
import json
import uuid

import paho.mqtt.client as mqtt

with open("variables.json") as f:
    config = json.load(f)

broker         = config["mqttBroker"]
port           = config["mqttPort"]
device_name    = config["Command"]["deviceName"]
resource       = config["Command"]["deviceResource"]
resource_value = config["Command"]["deviceResourceValue"]

cmd_topic = f"edgex/command/request/{device_name}/{resource}/set"
payload   = {resource: resource_value}

message = {
    "ApiVersion"   : "v3",
    "ContentType"  : "application/json",
    "CorrelationID": str(uuid.uuid4()),
    "RequestId"    : str(uuid.uuid4()),
    "Payload"      : base64.b64encode(json.dumps(payload).encode()).decode()
}

client = mqtt.Client()
client.connect(broker, port)
client.publish(cmd_topic, json.dumps(message))
print(f"Command sent to {device_name}: {resource} = {resource_value}")
client.disconnect()
