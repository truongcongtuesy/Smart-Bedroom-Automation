# Firmware (ESP32)

**Owner:** Person 1 (Hardware)

Contains the ESP32 code for sensor reading, actuator control, and the
automation rule engine.

## Contents

- `src/main.cpp` — full firmware: sensor reading (DHT22, PIR, LDR) with NaN
  handling, actuator control (relay/LED/buzzer), and the R1–R3 automation rule
  engine with `millis()` non-blocking timing.
- `diagram.json` — Wokwi circuit definition (ESP32 + all sensors/actuators).
- `platformio.ini` — PlatformIO build config (ESP32 Arduino framework, DHT lib).
- `wokwi.toml` — points the Wokwi simulator at the compiled firmware.

## Hardware

| Component | ESP32 Pin | Notes |
|---|---|---|
| DHT22 | GPIO 15 | Temp + humidity (single-wire data) |
| PIR | GPIO 27 | Digital motion, HIGH when detected |
| LDR | GPIO 34 | Analog input (ADC1, input-only pin) |
| Relay (fan) | GPIO 26 | Digital output, drives fan ON/OFF |
| LED | GPIO 25 | Digital output (via 220Ω resistor) |
| Buzzer | GPIO 33 | Digital output, short beep pulses |

_Wokwi project link:_ `[Add your saved Wokwi project URL here]`

## Automation Rules

See root [README](../README.md#automation-rules) for the rule table. R1
(fan) is closed-loop with a 2°C hysteresis band (30°C ON / 28°C OFF); R2 and
R3 are open-loop.

## How to Run (Wokwi)

1. Open the Wokwi project link above.
2. Click "Start Simulation".
3. Use the diagram controls to adjust temperature/light/motion and observe
   actuator responses in the console.
