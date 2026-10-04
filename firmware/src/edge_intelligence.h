// edge_intelligence.h - rule-based occupancy-pattern learning (Edge Intelligence).
//
// The PIR samples are aggregated into 24 hourly buckets. For the current hour the
// module keeps the ratio  (samples with motion) / (all samples).  That ratio puts the
// hour into one of four states, and the state changes how the firmware behaves:
//   HIGH   : ratio >  70 %  -> faster sensor polling, light stays on for a short hold time
//   LOW    : ratio <  20 %  -> slower sensor polling (saves power)
//   NORMAL : in between     -> default behaviour
//   LEARNING: fewer than EDGE_MIN_SAMPLES samples so far in this hour -> default behaviour
// Everything runs on the ESP32; nothing depends on the cloud.
#pragma once
#include <Arduino.h>

enum EdgeState { EDGE_LEARNING, EDGE_LOW, EDGE_NORMAL, EDGE_HIGH };

// Start the SNTP clock (non-blocking). Call once in setup().
void edgeBegin();

// Feed one PIR sample. Call once per sensor sample.
void edgeUpdate(unsigned long now, bool motion);

// Sensor polling interval for the current hour (ms).
unsigned long edgeSensorIntervalMs();

// True while the light should be held on after the last motion (HIGH hours only).
bool edgeKeepLit(unsigned long now, unsigned long lastMotionTime);

// State of the current hour, and its name for logging.
EdgeState edgeState();
const char* edgeStateName(EdgeState s);
