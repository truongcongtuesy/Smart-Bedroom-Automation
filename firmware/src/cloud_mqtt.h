// cloud_mqtt.h - Wi-Fi + MQTT telemetry to ThingSpeak (Network layer).
#pragma once
#include <Arduino.h>

// Start Wi-Fi (non-blocking) and configure the MQTT client. Call once in setup().
void cloudBegin();

// Keep Wi-Fi/MQTT alive. Non-blocking apart from a short, bounded connect
// attempt every 5 s while the broker is unreachable. Call every loop().
void cloudLoop(unsigned long now);

// Publish one entry (all four fields in a single message) if 20 s have elapsed,
// the sample is valid and the broker is connected.
// Returns true only when a message was actually published.
bool cloudPublishIfDue(unsigned long now, float temperature, float humidity,
                       int lightPercent, bool motionInWindow, bool sampleValid);
