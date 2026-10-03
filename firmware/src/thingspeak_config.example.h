// thingspeak_config.example.h
// ---------------------------------------------------------------------------
// Copy this file to  thingspeak_config.h  (same folder) and fill in the real
// values. thingspeak_config.h is listed in .gitignore and MUST NOT be committed.
// This example file contains placeholders only and is safe to commit.
// ---------------------------------------------------------------------------
#pragma once

// Wi-Fi. "Wokwi-GUEST" (empty password, channel 6) is the simulator's network.
#define WIFI_SSID        "Wokwi-GUEST"
#define WIFI_PASSWORD    ""
#define WIFI_CHANNEL     6

// ThingSpeak MQTT broker and channel.
#define TS_MQTT_HOST     "mqtt3.thingspeak.com"
#define TS_MQTT_PORT     1883
#define TS_CHANNEL_ID    "3470780"

// Per-device MQTT credentials from ThingSpeak (MQTT Devices page).
#define TS_MQTT_CLIENT_ID  "PASTE_CLIENT_ID_HERE"
#define TS_MQTT_USERNAME   "PASTE_USERNAME_HERE"
#define TS_MQTT_PASSWORD   "PASTE_PASSWORD_HERE"
