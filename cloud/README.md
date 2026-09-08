# Cloud Integration (MQTT + ThingSpeak)

**Owner:** Person 2 (Cloud/Comm.)

Contains the MQTT publish logic and ThingSpeak configuration.

## Contents

- `mqtt_publish.ino` / `.cpp` — connects to ThingSpeak's MQTT broker and
  publishes sensor readings
- `thingspeak_config.h` — Channel ID, field mapping, MQTT credentials
  (**do not commit real passwords — use a `.env`/local-only config, see
  `.gitignore`**)

## ThingSpeak Setup

- **Channel ID:** 3470780
- **Fields:**
  | Field | Meaning |
  |---|---|
  | Field1 | Room Temperature (°C) |
  | Field2 | Humidity (%) |
  | Field3 | Ambient Light Level |
  | Field4 | Occupancy (PIR: 0/1) |
- **Broker:** `mqtt3.thingspeak.com`
- **Topic:** `channels/3470780/publish`
- **Auth:** MQTT Client ID + Username + MQTT API Key (password) — from
  ThingSpeak Device Credentials, not the HTTP Write API Key.
- **Publish interval:** ≥ 15 seconds (ThingSpeak free-tier rate limit)

## Security

MQTT connection requires username/password authentication (ThingSpeak
Device Credentials), satisfying the project's minimum security
requirement. See root README and final report §9 for details.

## Dashboard

Live dashboard: `[ThingSpeak channel public view link]`
Screenshot: see `docs/report-drafts/` for the version used in the report.
