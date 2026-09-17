"""
Reads device definitions from a JSON file and publishes each one
to the EdgeX gateway via MQTT, registering them in the system.

Usage:
    python add_device_from_json.py
    python add_device_from_json.py -j my_devices.json
"""

import argparse
import json
import time

import paho.mqtt.client as mqtt

parser = argparse.ArgumentParser(description="Register devices from a JSON file via MQTT.")
parser.add_argument("-j", "--json", default="devices_list.json",
                    help="Path to the JSON file containing the device list.")
args = parser.parse_args()

with open(args.json) as f:
    devices = json.load(f)["devices"]

with open("variables.json") as f:
    config = json.load(f)

broker = config["mqttBroker"]
port   = config["mqttPort"]
topic  = config["mqttTopicNewDevice"]

client = mqtt.Client()
client.connect(broker, port)

for device in devices:
    client.publish(topic, json.dumps(device))
    print(f"Registered: {device['name']} ({device['profileName']})")
    time.sleep(0.5)

client.disconnect()
