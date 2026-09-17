# Hydroponics ESP32 — Project Handover

**Last updated:** 2026-06-19
**Status:** Working prototype on a breadboard (ESP1). 4 of 7 sensor types verified end-to-end; 3 pending hardware (5 V power + voltage dividers).

---

## 1. What this project is

An IoT monitoring/control system for a hydroponics setup. Each **ESP32** board reads a set of
sensors (temperature, humidity, water quality, distance, light, gas) and controls **4 relays**
(pumps / valves / lights). Readings are published over **MQTT**; relay commands are received over
MQTT. The backend is an **EdgeX** deployment that registers the devices and routes the data.

The design supports **up to 4 ESPs** (ESP1–ESP4), each with its own copy of the sensor set. Only
**ESP1** has been built and tested so far.

### The three moving parts

| Part | Where | Role |
|------|-------|------|
| **ESP32 firmware** | `src/` (C++, PlatformIO) | Reads sensors, drives relays, talks MQTT/HTTP |
| **Python config scripts** | repo root (`*.py`) | Register/delete devices, profiles, streams, rules; send commands |
| **Backend (pre-existing)** | EdgeX + Mosquitto brokers on the LAN | Stores devices, bridges MQTT, runs rules (eKuiper) |

---

## 2. Network / IP map (IMPORTANT — these are hardcoded)

> The new team will almost certainly be on a different network. **These values must be changed.**

### ESP firmware — edit `src/main.cpp` (lines ~20–26)

```cpp
const char* ssid        = "LIJO-SMU";                       // WiFi SSID
const char* password    = "3@7984A5B";                      // WiFi password
const char* mqtt_server = "192.168.10.102";                 // MQTT broker the ESP publishes to
const int   mqtt_port   = 31883;
const char* mqtt_topic  = "incoming/data";                  // base topic for sensor data
const char* esp_name    = "ESP1";                           // ESP identity (ESP1..ESP4)
const char* devices_url = "http://192.168.10.102:30300/configs";  // HTTP config server
```

The ESP does two things on the network:
1. **HTTP GET** `http://192.168.10.102:30300/configs/ESP1` every 30 s to fetch its device list.
2. **MQTT publish** sensor data to `192.168.10.102:31883` under `incoming/data/<device>/<reading>`.

### Python scripts — edit `variables.json`

```jsonc
"mqttBroker" : "192.168.10.103",   // broker the Python scripts publish device config to (EdgeX)
"mqttPort"   : 31883,
"metadataIp" : "10.152.183.17",    // EdgeX core-metadata
"eKuiperIp"  : "10.152.183.183"    // eKuiper rules engine
```

> ⚠️ Note the **two brokers**: the ESP uses `.102`, the Python scripts use `.103`. This is intentional
> in the current backend (the ESP's raw MQTT vs the EdgeX core), but if you redeploy the backend you
> will likely want them pointed at the same place. Confirm with whoever owns the EdgeX deployment.

---

## 3. Hardware

### 3.1 Bill of materials (per ESP)

- 1 × ESP32 dev board (**upesy WROOM**, `board = upesy_wroom`)
- 1 × DHT22 / AM2302 temperature+humidity sensor (3-pin breakout)
- 1 × DFRobot Analog Ambient Light Sensor V2.1
- 1 × MQ-7 carbon-monoxide gas sensor (Flying-Fish board)
- 1 × TDS (total dissolved solids) sensor
- 1 × pH sensor (4502C board + probe)
- 1 × HC-SR04 ultrasonic distance sensor
- 1 × 4-channel relay module
- Resistors for voltage dividers (see 3.4)
- Breadboard + jumper wires

### 3.2 Pin map (ESP1)

| GPIO | Component | Sensor pin | Power | Divider? |
|-----:|-----------|-----------|:-----:|:--------:|
| 4    | DHT22     | DATA       | 3.3 V | no (10 kΩ pull-up to 3.3 V) |
| 36   | Light V2.1| AOUT       | 3.3 V | no |
| 32   | MQ-7      | DOUT       | 5 V   | **yes** |
| 33   | MQ-7      | AOUT       | 5 V   | **yes** |
| 34   | TDS       | AOUT       | 5 V   | **yes** |
| 35   | pH 4502C  | AOUT       | 5 V   | **yes** |
| 23   | HC-SR04   | TRIG       | 5 V   | no (ESP output) |
| 22   | HC-SR04   | ECHO       | 5 V   | **yes** |
| 21   | Relay 1   | IN1        | 5 V   | no (ESP output) |
| 18   | Relay 2   | IN2        | 5 V   | no |
| 5    | Relay 3   | IN3        | 5 V   | no |
| 27   | Relay 4   | IN4        | 5 V   | no |

> **DHT22 was moved from GPIO 2 to GPIO 4.** GPIO 2 is a boot-strap pin and was unreliable. The
> firmware uses `DHT22` (not DHT21) — see `src/temperatureHumiditySensor.cpp`.

### 3.3 Power

- **3.3 V** and **5 V** both come from the **ESP32 dev board itself** when powered over USB:
  - `3V3` pin → DHT22, light sensor
  - `5V` / `VIN` pin → MQ-7, TDS, pH, HC-SR04, relays  *(this pin carries 5 V from the USB cable)*
- **No external power supply is required for sensors.** All sensors share a **common ground** with the ESP.
- ⚠️ When the **4 relays** are added, a laptop USB port may brown out (each coil ~70 mA). Use a
  5 V phone charger / powered USB hub for the relay board's VCC (still sharing GND with the ESP).

### 3.4 Voltage dividers (5 V signals → safe 3.3 V)

ESP32 ADC/GPIO pins tolerate **max 3.3 V**. Every **5 V signal going *into* the ESP** must pass
through a divider. Outputs from the ESP (TRIG, relay INs) do not.

Using the resistors on hand (**510 Ω** and **2 kΩ**):

```
sensor signal ──[510Ω]──[510Ω]──┬────────► ESP32 GPIO
                (1020Ω top)      │
                              [2kΩ]  (bottom)
                                 │
                                GND

Output = 5 V × 2000 / (2000 + 1020) ≈ 3.31 V  ✅
```

- **Top resistor** = two 510 Ω in series (= 1020 Ω). *Do not use a single 510 Ω — that gives 3.98 V (unsafe).*
- **Bottom resistor** = one 2 kΩ to GND.
- Needed on **5 dividers**: GPIO 22 (ECHO), 32 (MQ-7 DOUT), 33 (MQ-7 AOUT), 34 (TDS), 35 (pH).
  Total: **10 × 510 Ω + 5 × 2 kΩ** per ESP.

> **Calibration note:** the divider feeds the ESP **0.66 ×** the real voltage. To recover true values,
> multiply the read voltage by **1.51** in the driver, e.g. in `phSensor.cpp`:
> `float voltage = analogRead(_analogPin) * (3.3f / 4095.0f) * 1.51f;`
> The same applies to the TDS voltage calculation. (Not yet applied — see Pending Tasks.)

---

## 4. Software setup & dependencies

### 4.1 ESP firmware (PlatformIO)

1. Install **VS Code** + the **PlatformIO IDE** extension (or PlatformIO Core CLI: `pip install platformio`).
2. Open the project folder in VS Code. PlatformIO auto-installs the libraries listed in
   `platformio.ini`:
   - ArduinoJson `^6.21.2`
   - PubSubClient `^2.8`
   - Adafruit Unified Sensor `^1.1.9`
   - DHT sensor library `^1.4.4`
   - ezButton `^1.0.4`
   - MQUnifiedsensor `^3.0.0`
   - DallasTemperature `^3.11.0`
   - NTPClient `^3.2.1`
3. ⚠️ `platformio.ini` has `lib_extra_dirs = ~/Documents/Arduino/libraries`. If you get "library not
   found", either create that folder or remove that line and let PlatformIO manage libraries.

### 4.2 Python scripts

- **Python 3** + **paho-mqtt**:
  ```bash
  pip install paho-mqtt
  ```

### 4.3 Monitoring tools

- **MQTT Explorer** (GUI) — connect to the broker to watch live data (see §6).
- (optional) **mosquitto-clients** for CLI: `sudo apt install mosquitto-clients`

---

## 5. How to build, flash, and run

### 5.1 Flash the ESP32

1. Edit `src/main.cpp` — set WiFi + broker IPs + `esp_name` (§2).
2. Plug the ESP32 in via USB. (Install CP210x/CH340 USB driver if the COM port doesn't appear.)
3. In the PlatformIO sidebar → `upezy_wroom` → **Upload and Monitor**.
   - CLI equivalent: `pio run -t upload && pio device monitor`
4. Expected serial output: WiFi connect → `MQTT connected.` → device list → readings.

> **Note on project structure:** the entry point is `src/main.cpp` (contains `setup()`/`loop()`).
> An earlier `src/main/main.ino` was renamed to `src/main.cpp` — PlatformIO only compiles `.cpp`
> files directly under `src/`.

### 5.2 Register devices in the backend

From the repo root (with the broker reachable, §2):

```bash
python add_profile.py             # push device profiles (device_profile.json)
python add_device_from_json.py    # register all devices (devices_list.json)
```

To wipe and re-register (e.g. after changing a GPIO):

```bash
python delete_all_devices.py
python add_device_from_json.py
```

Optional rules/streams (eKuiper):
```bash
python add_stream.py
python add_rule.py
```

### 5.3 Send a relay command

Edit the `"Command"` block in `variables.json` (deviceName / deviceResource / value), then:

```bash
python command_device.py
```

---

## 6. How to monitor the data

1. Open **MQTT Explorer** → connect to the **ESP's broker** (`192.168.10.102:31883` — *no* TLS, *no* auth).
2. Expand the tree: `incoming` → `data` → one branch per device. Each reading updates in place:

```
incoming/data/
  dht-esp1-.../temperature, /humidity
  lgt-esp1-.../intensity
  phx-esp1-.../Ph value
  son-esp1-.../distance
  tds-esp1-.../TDS value
  mq7-esp1-.../value, /limit
  re1-esp1-.../Status  (re1..re4)
```

3. CLI alternative (watch one sensor):
   ```bash
   mosquitto_sub -h 192.168.10.102 -p 31883 -t 'incoming/data/phx-esp1-#' -v
   ```

> The EdgeX-formatted copy of the same data appears on the **other broker** (`192.168.10.103`)
> under the `edgex_pi` topic.

---

## 7. Current implementation status

### ✅ Working & verified (ESP1)
- WiFi + MQTT connectivity, auto-reconnect, ESP restart on failure.
- Dynamic device management: ESP fetches its device list over HTTP and creates/updates/removes
  sensor objects at runtime (add/remove a device without re-flashing).
- **DHT22** (temp 28.2 °C, humidity 44 %) — correct.
- **TDS** — correct (reads 0 in air; rises in water).
- **Light sensor** — working (saturates at 4095 under bright light).
- **Relays** — status reporting works; command path implemented (`command_device.py`).
- Full Python tooling for device/profile/stream/rule registration and commands.

### ⚠️ Partially working / needs calibration
- **pH** — pipeline publishes, but value is uncalibrated and needs the probe in solution + the
  `× 1.51` divider correction. Calibrate against pH 4 and pH 7 buffer solutions (`M` slope in
  `phSensor.h`).

### ❌ Not working yet (hardware)
- **MQ-7** — reads 0 because its heater needs **5 V** (was on 3.3 V). Move VCC to the `5V`/`VIN` pin.
- **HC-SR04** — invalid distance (1000 cm) at 3.3 V with no divider. Needs **5 V** power + a divider
  on ECHO (GPIO 22).

---

## 8. Pending tasks (for the new team)

1. **Add the 5 voltage dividers** (GPIO 22, 32, 33, 34, 35) — see §3.4. Required for accurate &
   safe analog readings.
2. **Power MQ-7, HC-SR04, TDS, pH from 5 V** (the `5V`/`VIN` pin), not 3.3 V.
3. **Apply the `× 1.51` divider correction** in `phSensor.cpp` and `tdsSensor.cpp` after dividers
   are installed.
4. **Calibrate** pH (buffer solutions) and TDS (known reference solution).
5. **Restore production polling intervals** — they are currently **5000 ms** for testing in every
   sensor `loop()`. Suggested originals: light 100 s, DHT 300 s, pH 400 s, MQ-7 600 s, ultrasonic
   200 s, TDS publish 240 s. (Search each `src/*.cpp` for `_lastUpdateMillis <` / `_lastPublishMillis >=`.)
6. **Wire and test the 4 relays** (use a 5 V supply for the relay board to avoid USB brown-out).
7. **Replicate ESP1 onto ESP2–ESP4** (change `esp_name` in `main.cpp`, re-flash; devices already
   defined in `devices_list.json`).
8. **Unify the two MQTT brokers** (.102 vs .103) if the backend is redeployed.
9. Optional: change the firmware to publish each sensor under its own subtopic for easier debugging
   (currently the EdgeX bridge merges everything onto `edgex_pi` on the .103 broker).

---

## 9. File reference

### Firmware (`src/`)
- `main.cpp` — entry point: WiFi/MQTT setup, config fetch loop, command routing.
- `connectToWifi.*` — blocking WiFi connect with restart-on-failure.
- `deviceManager.*` — owns all device objects; diff-syncs them against the fetched config.
- `deviceFactory.*` — maps a profile name → concrete device class.
- `device.h`, `sensor.*` — base classes; `sensor.cpp` has the MQTT publish helpers.
- Sensor drivers: `temperatureHumiditySensor` (DHT22), `analogLightV2`, `mq7Sensor`, `tdsSensor`,
  `phSensor`, `ultraSonicSensor`, `waterTemp` (DS18B20), `temperatureSensor`, `moistureSensor`,
  `limitSwitch`, `timer` (NTP).
- `relayActuator.*` — relay control (active-LOW) + status publishing.

> Drivers for `waterTemp`, `temperatureSensor`, `moistureSensor`, `limitSwitch`, `timer` exist but
> are **not** in the current `devices_list.json` — they are available for future use.

### Config / data (repo root)
- `platformio.ini` — board, framework, libraries.
- `variables.json` — broker IPs, MQTT topic names, and the payloads used by the Python scripts.
- `devices_list.json` — all devices for ESP1–ESP4 (name, profile, GPIOs).
- `device_profile.json` — EdgeX device profiles (resources, value types, units).

### Python tooling (repo root)
- `add_profile.py`, `add_device_from_json.py`, `add_stream.py`, `add_rule.py` — register.
- `delete_*.py` — remove devices / profiles / streams / rules (parameters from `variables.json`).
- `command_device.py` — send a relay command (EdgeX v3, base64 payload).
- `mqtt_device_simulator.py` — publish fake device data for testing without hardware.

### Other
- `hydroponics_setting_up_esp.pdf` — original setup guide from the previous team.
- `HANDOVER.md` — this document.
