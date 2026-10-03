# Cloud Integration (MQTT + ThingSpeak)

**Owner:** Person 2 (Cloud/Comm.)

Contains the MQTT publish logic and ThingSpeak configuration.

## Contents

PlatformIO only compiles `firmware/src/`, so the MQTT code lives there:

- `firmware/src/cloud_mqtt.h` / `cloud_mqtt.cpp` — non-blocking Wi-Fi + MQTT
  client; publishes all four fields in one message every 20 s.
- `firmware/src/thingspeak_config.example.h` — placeholders only (safe to
  commit). **Copy it to `thingspeak_config.h` and fill in the real Wi-Fi/MQTT
  values. `thingspeak_config.h` is git-ignored — never commit it.**
- `firmware/platformio.ini` — adds the `PubSubClient` library.

## Payload

One publish carries the whole state:

```
Topic:   channels/3470780/publish
Payload: field1=<temp C>&field2=<humidity %>&field3=<light 0-100>&field4=<motion 0/1>&status=MQTTPUBLISH
```

Light is normalised from the raw 0-4095 ADC value to 0-100 % on the device.
`field4` is 1 if the PIR fired at any point since the previous publish.

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
