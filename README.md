# Smart Bedroom Automation System

**Course:** 3707ICT — Smart IoT Automation System
**Team:** [Add group member names here]

An ESP32-based IoT system that monitors temperature, humidity, ambient light,
and occupancy in a bedroom, automatically controls a fan/light/buzzer, and
reports live + historical data to a ThingSpeak cloud dashboard via MQTT.

## Repository Structure

| Folder | Owner | Contents |
|---|---|---|
| [`firmware/`](./firmware) | Person 1 (Hardware) | ESP32 code: sensors, actuators, automation rules |
| [`cloud/`](./cloud) | Person 2 (Cloud/Comm.) | MQTT + ThingSpeak connection code and config |
| [`edge-intelligence/`](./edge-intelligence) | You (Integration) | Occupancy-pattern rule-based intelligence module |
| [`docs/circuit-diagram/`](./docs/circuit-diagram) | Person 1 | Wokwi schematic / circuit diagram screenshots |
| [`docs/report-drafts/`](./docs/report-drafts) | Shared | Working drafts of the final report |
| [`demo-video/`](./demo-video) | You (Integration) | Link/script for the 5-minute demo video |

## System Architecture

- **Perception:** DHT22 (temp/humidity), PIR (motion), LDR (light)
- **Network:** ESP32 Wi-Fi + MQTT to ThingSpeak
- **Processing:** ESP32 automation rule engine + edge intelligence
- **Application:** ThingSpeak dashboard (Channel ID: 3470780)

## Automation Rules

| Rule | Condition | Action |
|---|---|---|
| R1 | Temp > 30°C | Fan ON (closed-loop, 2°C hysteresis) |
| R2 | Dark + motion detected | LED ON (open-loop) |
| R3 | No motion > 5 min | All actuators OFF (open-loop) |

## Getting Started

1. Clone this repo: `git clone <repo-url>`
2. Firmware setup: see [`firmware/README.md`](./firmware/README.md)
3. Cloud setup: see [`cloud/README.md`](./cloud/README.md)
4. Edge intelligence: see [`edge-intelligence/README.md`](./edge-intelligence/README.md)

## Final Report

The full report is maintained in [`docs/report-drafts/`](./docs/report-drafts) and
submitted separately per course requirements.
