// Smart Bedroom Automation - ESP32 (Group 10, 3707ICT)
// R1 temperature (closed-loop), R2 light+motion, R3 energy saving. millis() timing.
 
#include <Arduino.h>
#include <DHTesp.h>
#include "cloud_mqtt.h"   // Cloud/Comm.: Wi-Fi + MQTT to ThingSpeak
 
#define DHT_PIN     15
#define PIR_PIN     27
#define LDR_PIN     34
#define RELAY_PIN   26
#define LED_PIN     25
#define BUZZER_PIN  33
 
const float TEMP_ON_THRESHOLD   = 30.0f;
const float TEMP_OFF_THRESHOLD  = 28.0f;
const int   LDR_DARK_THRESHOLD  = 3000;   // raw ADC. Wokwi LDR module: HIGH reading = DARK (0.1 lux ~ 4063, 500 lux ~ 1000)
const unsigned long NO_MOTION_TIMEOUT_MS    = 5UL * 60UL * 1000UL;
const unsigned long SENSOR_READ_INTERVAL_MS = 2500;
const unsigned long BUZZER_BEEP_MS          = 150;
 
DHTesp dht;
 
float temperature = NAN;
float humidity = NAN;
int   lightLevel = 0;
bool  motionDetected = false;
bool  sensorFault = false;
bool  motionInWindow = false;   // PIR seen since the last cloud publish
 
bool fanState = false;
bool ledState = false;
bool energySavingActive = false;
 
unsigned long lastSensorRead = 0;
unsigned long lastMotionTime = 0;
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;
 
void readSensors(unsigned long now);
void evaluateAutomationRules(unsigned long now);
void setFan(bool on);
void setLed(bool on);
void triggerBuzzerBeep(unsigned long now);
void updateBuzzer(unsigned long now);
 
void setup() {
  Serial.begin(115200);
  pinMode(DHT_PIN, INPUT_PULLUP);
  dht.setup(DHT_PIN, DHTesp::DHT22);
  delay(2000);
 
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
 
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
 
  lastMotionTime = millis();
  cloudBegin();
  Serial.println("[BOOT] Smart Bedroom Automation ready");
}
 
void loop() {
  unsigned long now = millis();
 
  readSensors(now);
  evaluateAutomationRules(now);
  updateBuzzer(now);
 
  // Cloud/Comm.: telemetry. Never blocks the rules above.
  cloudLoop(now);
  int lightPercent = 100 - constrain(map(lightLevel, 0, 4095, 0, 100), 0, 100);   // 100 = bright, 0 = dark
  bool sampleValid = !sensorFault && !isnan(temperature) && !isnan(humidity);
  if (cloudPublishIfDue(now, temperature, humidity, lightPercent, motionInWindow, sampleValid)) {
    motionInWindow = false;   // start a new 20 s occupancy window
  }
}
 
void readSensors(unsigned long now) {
  if (now - lastSensorRead < SENSOR_READ_INTERVAL_MS) return;
  lastSensorRead = now;
 
  TempAndHumidity reading = dht.getTempAndHumidity();
  float t = reading.temperature;
  float h = reading.humidity;
 
  if (isnan(t) || isnan(h)) {
    sensorFault = true;
    Serial.printf("[WARN] DHT22 fault (%s) - safe state\r\n", dht.getStatusString());
  } else {
    sensorFault = false;
    temperature = t;
    humidity = h;
  }
 
  lightLevel = analogRead(LDR_PIN);
  motionDetected = digitalRead(PIR_PIN) == HIGH;
 
  if (motionDetected) {
    lastMotionTime = now;
    motionInWindow = true;
  }
 
  Serial.printf("T=%.1fC H=%.1f%% Light=%d Motion=%d Fault=%d\r\n",
                temperature, humidity, lightLevel, motionDetected, sensorFault);
}
 
void evaluateAutomationRules(unsigned long now) {
  // R3: no motion for 5 min -> all off
  if ((now - lastMotionTime) > NO_MOTION_TIMEOUT_MS) {
    if (!energySavingActive) {
      energySavingActive = true;
      triggerBuzzerBeep(now);
      Serial.println("[R3] No motion -> actuators OFF");  // println already emits CRLF
    }
    setFan(false);
    setLed(false);
    return;
  }
  energySavingActive = false;
 
  // R1: temperature, 2C hysteresis
  if (sensorFault) {
    setFan(false);
  } else if (!fanState && temperature > TEMP_ON_THRESHOLD) {
    setFan(true);
  } else if (fanState && temperature < TEMP_OFF_THRESHOLD) {
    setFan(false);
  }
 
  // R2: dark AND motion
  setLed(lightLevel > LDR_DARK_THRESHOLD && motionDetected);
}
 
void setFan(bool on) {
  if (fanState == on) return;
  fanState = on;
  digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  Serial.printf("[R1] Fan %s\r\n", on ? "ON" : "OFF");
}
 
void setLed(bool on) {
  if (ledState == on) return;
  ledState = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  Serial.printf("[R2] LED %s\r\n", on ? "ON" : "OFF");
}
 
void triggerBuzzerBeep(unsigned long now) {
  buzzerActive = true;
  buzzerStartTime = now;
  digitalWrite(BUZZER_PIN, HIGH);
}
 
void updateBuzzer(unsigned long now) {
  if (buzzerActive && (now - buzzerStartTime >= BUZZER_BEEP_MS)) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
  }
}