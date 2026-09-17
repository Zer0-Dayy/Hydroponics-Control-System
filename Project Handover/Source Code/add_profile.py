"""
Reads device profile definitions from device_profile.json and publishes each one
to the EdgeX gateway via MQTT, registering the profiles in the system.
Profiles must exist before devices that use them can be registered.
"""

import json
import time
import uuid

import paho.mqtt.client as mqtt

with open("device_profile.json") as f:
    profiles = json.load(f)["deviceProfiles"]

with open("variables.json") as f:
    config = json.load(f)

broker = config["mqttBroker"]
port   = config["mqttPort"]
topic  = config["mqttTopicNewDeviceProfile"]

client = mqtt.Client()
client.connect(broker, port)

for profile in profiles:
    envelope = [{
        "requestId" : str(uuid.uuid4()),
        "apiVersion": "v2",
        "profile"   : profile
    }]
    client.publish(topic, json.dumps(envelope))
    print(f"Registered profile: {profile['name']}")
    time.sleep(0.5)

client.disconnect()
