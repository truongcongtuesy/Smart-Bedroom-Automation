// cloud_mqtt.cpp - Wi-Fi + MQTT telemetry to ThingSpeak.
// Automation decisions never depend on this file: if Wi-Fi or the broker is
// down, the rules in main.cpp keep running locally.

#include "cloud_mqtt.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "thingspeak_config.h"   // real credentials; git-ignored

static const unsigned long PUBLISH_INTERVAL_MS   = 20000;  // ThingSpeak free tier: >= 15 s
static const unsigned long RECONNECT_INTERVAL_MS = 5000;   // retry timer while broker is down
static const uint16_t      SOCKET_TIMEOUT_S      = 2;      // caps a failed connect attempt

static WiFiClient   wifiClient;
static PubSubClient mqtt(wifiClient);

static char          publishTopic[48];
static unsigned long lastPublishTime     = 0;
static bool          hasPublished        = false;
static unsigned long lastReconnectTry    = 0;
static bool          wifiWasConnected    = false;

void cloudBegin() {
  snprintf(publishTopic, sizeof(publishTopic), "channels/%s/publish", TS_CHANNEL_ID);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);   // returns immediately

  mqtt.setServer(TS_MQTT_HOST, TS_MQTT_PORT);
  mqtt.setSocketTimeout(SOCKET_TIMEOUT_S);
  Serial.println("[CLOUD] Wi-Fi connecting in background");
}

void cloudLoop(unsigned long now) {
  bool wifiUp = (WiFi.status() == WL_CONNECTED);
  if (wifiUp != wifiWasConnected) {                      // log link changes only
    wifiWasConnected = wifiUp;
    if (wifiUp) {
      Serial.printf("[CLOUD] Wi-Fi up, IP %s\r\n", WiFi.localIP().toString().c_str());
    } else {
      Serial.println("[CLOUD] Wi-Fi down - publishing paused, automation unaffected");
    }
  }
  if (!wifiUp) return;                                   // skip publish step while link is down

  if (!mqtt.connected()) {
    if (now - lastReconnectTry >= RECONNECT_INTERVAL_MS) {
      lastReconnectTry = now;
      Serial.printf("[CLOUD] MQTT connecting to %s ...\r\n", TS_MQTT_HOST);
      if (mqtt.connect(TS_MQTT_CLIENT_ID, TS_MQTT_USERNAME, TS_MQTT_PASSWORD)) {
        Serial.println("[CLOUD] MQTT connected");
      } else {
        // rc 4/5 = bad credentials / not authorised, -2 = broker unreachable
        Serial.printf("[CLOUD] MQTT connect failed, rc=%d\r\n", mqtt.state());
      }
    }
    return;
  }
  mqtt.loop();
}

bool cloudPublishIfDue(unsigned long now, float temperature, float humidity,
                       int lightPercent, bool motionInWindow, bool sampleValid) {
  if (hasPublished && (now - lastPublishTime < PUBLISH_INTERVAL_MS)) return false;
  if (!sampleValid) return false;          // never publish NaN / faulty readings
  if (WiFi.status() != WL_CONNECTED) return false;   // link down: skip, don't rely on a stale socket
  if (!mqtt.connected()) return false;

  char payload[96];
  snprintf(payload, sizeof(payload),
           "field1=%.2f&field2=%.2f&field3=%d&field4=%d&status=MQTTPUBLISH",
           temperature, humidity, lightPercent, motionInWindow ? 1 : 0);

  bool ok = mqtt.publish(publishTopic, payload);
  lastPublishTime = now;                   // honour the rate limit even if this attempt failed
  hasPublished = true;
  Serial.printf("[CLOUD] publish %s: %s\r\n", ok ? "OK" : "FAILED", payload);
  return ok;
}
