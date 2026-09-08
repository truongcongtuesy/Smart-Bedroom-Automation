# Firmware (ESP32)

**Owner:** Person 1 (Hardware)

Contains the ESP32 code for sensor reading, actuator control, and the
automation rule engine.

## Contents

- `src/` — Arduino/ESP32 source files
  - `sensors.ino` / `sensors.cpp` — DHT22, PIR, LDR reading + calibration
  - `actuators.ino` / `actuators.cpp` — Relay (fan), LED, Buzzer control
  - `automation_rules.ino` / `.cpp` — R1–R3 rule logic, hysteresis
  - `main.ino` — setup()/loop(), non-blocking millis() timing

## Hardware

| Component | ESP32 Pin | Notes |
|---|---|---|
| DHT22 | GPIO 15 | Temp + humidity |
| PIR | GPIO 27 | Digital motion |
| LDR | GPIO 34 | Analog (ADC pin) |
| Relay (fan) | GPIO ? | Fill in |
| LED | GPIO ? | Fill in |
| Buzzer | GPIO ? | Fill in |

_Update the pin table above and link the Wokwi project here:_
`[Wokwi project link]`

## Automation Rules

See root [README](../README.md#automation-rules) for the rule table. R1
(fan) is closed-loop with a 2°C hysteresis band (30°C ON / 28°C OFF); R2 and
R3 are open-loop.

## How to Run (Wokwi)

1. Open the Wokwi project link above.
2. Click "Start Simulation".
3. Use the diagram controls to adjust temperature/light/motion and observe
   actuator responses in the console.
