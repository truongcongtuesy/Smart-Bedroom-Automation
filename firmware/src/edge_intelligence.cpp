// edge_intelligence.cpp - see edge_intelligence.h
#include "edge_intelligence.h"
#include <time.h>

// ---- tunable parameters (override with -D flags at build time) -------------------
#ifndef EDGE_HIGH_RATIO_PCT
#define EDGE_HIGH_RATIO_PCT 70        // ratio above this  -> HIGH occupancy hour
#endif
#ifndef EDGE_LOW_RATIO_PCT
#define EDGE_LOW_RATIO_PCT 20         // ratio below this  -> LOW occupancy hour
#endif
#ifndef EDGE_MIN_SAMPLES
#define EDGE_MIN_SAMPLES 20           // samples needed before a ratio is trusted (~50 s)
#endif
#ifndef EDGE_LIGHT_HOLD_MS
#define EDGE_LIGHT_HOLD_MS 30000UL    // keep light on this long after last motion (HIGH hours)
#endif
// Demo clock: define EDGE_DEMO_HOUR_MS (e.g. 30000) so one "hour" lasts that many ms.
// It lets the learning be shown in a minute instead of a day. Not defined in normal builds.

static const unsigned long POLL_HIGH_MS   = 2000;   // DHT22 minimum sampling period
static const unsigned long POLL_NORMAL_MS = 2500;   // same as SENSOR_READ_INTERVAL_MS in main.cpp
static const unsigned long POLL_LOW_MS    = 5000;

static uint32_t   checks[24];          // samples seen in each hour of the day
static uint32_t   hits[24];            // ... of which the PIR reported motion
static int        lastHour  = -1;
static EdgeState  curState  = EDGE_LEARNING;
#ifndef EDGE_DEMO_HOUR_MS
static bool       ntpLogged = false;
#endif

void edgeBegin() {
#ifndef EDGE_DEMO_HOUR_MS
  configTzTime("AEST-10", "pool.ntp.org", "time.google.com");   // Brisbane, UTC+10, no DST
#endif
}

static int currentHour(unsigned long now) {
#ifdef EDGE_DEMO_HOUR_MS
  return (int)((now / EDGE_DEMO_HOUR_MS) % 24UL);               // accelerated demo clock
#else
  time_t t = time(nullptr);
  if (t > 1700000000L) {                                        // SNTP has set the clock
    struct tm lt;
    localtime_r(&t, &lt);
    if (!ntpLogged) {
      ntpLogged = true;
      Serial.printf("[EDGE] clock synchronised, local hour %02d\r\n", lt.tm_hour);
    }
    return lt.tm_hour;
  }
  return (int)((now / 3600000UL) % 24UL);                       // fallback: hour of uptime
#endif
}

static EdgeState stateFor(int h) {
  if (h < 0 || checks[h] < EDGE_MIN_SAMPLES) return EDGE_LEARNING;
  uint32_t pct = (100UL * hits[h]) / checks[h];
  if (pct > EDGE_HIGH_RATIO_PCT) return EDGE_HIGH;
  if (pct < EDGE_LOW_RATIO_PCT)  return EDGE_LOW;
  return EDGE_NORMAL;
}

const char* edgeStateName(EdgeState s) {
  switch (s) {
    case EDGE_HIGH:   return "HIGH";
    case EDGE_LOW:    return "LOW";
    case EDGE_NORMAL: return "NORMAL";
    default:          return "LEARNING";
  }
}

void edgeUpdate(unsigned long now, bool motion) {
  int h = currentHour(now);
  checks[h]++;
  if (motion) hits[h]++;

  EdgeState s = stateFor(h);
  if (h != lastHour || s != curState) {                         // log hour or state changes only
    uint32_t pct = checks[h] ? (100UL * hits[h]) / checks[h] : 0;
    Serial.printf("[EDGE] hour %02d: %s (%u/%u motion samples = %u%%)\r\n",
                  h, edgeStateName(s), (unsigned)hits[h], (unsigned)checks[h], (unsigned)pct);
    lastHour = h;
    curState = s;
  }
}

EdgeState edgeState() { return curState; }

unsigned long edgeSensorIntervalMs() {
  if (curState == EDGE_HIGH) return POLL_HIGH_MS;
  if (curState == EDGE_LOW)  return POLL_LOW_MS;
  return POLL_NORMAL_MS;
}

bool edgeKeepLit(unsigned long now, unsigned long lastMotionTime) {
  return curState == EDGE_HIGH && (now - lastMotionTime) < EDGE_LIGHT_HOLD_MS;
}
