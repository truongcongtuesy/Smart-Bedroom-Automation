# 4. Circuit Diagram

*(Section owner: Hardware & Automation Lead)*

The system is built and simulated on an ESP32 DevKit-C in Wokwi. The full wiring
is defined in [`firmware/diagram.json`](../../firmware/diagram.json) and can be opened
directly in the Wokwi simulator (`firmware/wokwi.toml`) alongside the firmware in
[`firmware/src/main.cpp`](../../firmware/src/main.cpp).

> Insert a screenshot of the Wokwi diagram here before submitting the report
> (open `diagram.json` in the Wokwi VS Code extension or wokwi.com and export a PNG).

## Pin connection table

| Component        | Signal        | ESP32 Pin | Notes                                   |
|-------------------|---------------|-----------|------------------------------------------|
| DHT22             | VCC / GND     | 3V3 / GND | Power                                    |
| DHT22             | Data (SDA)    | GPIO 15   | Single-wire digital data line            |
| PIR motion sensor | VCC / GND     | 3V3 / GND | Power                                    |
| PIR motion sensor | OUT           | GPIO 27   | Digital HIGH when motion detected        |
| LDR (photoresistor) | VCC / GND   | 3V3 / GND | Power                                    |
| LDR (photoresistor) | AOUT        | GPIO 34   | Analog input (ADC1, 0–4095)              |
| Relay module (fan)| VCC / GND     | 5V / GND  | Power (relay coil driver needs 5V)       |
| Relay module (fan)| IN            | GPIO 26   | Digital output, drives fan ON/OFF        |
| LED (bedroom light)| Anode / Cathode | GPIO 25 / GND (via 220Ω resistor) | Digital output |
| Buzzer            | + / −         | GPIO 33 / GND | Digital output, short beep pulses    |

All sensors run on 3.3V; the relay module is powered from the ESP32's 5V rail since
typical relay driver boards need 5V for the opto-coupler/transistor stage, while the
ESP32 GPIO logic level (3.3V) is still sufficient to drive the relay's `IN` pin.

# 5. Automation Logic

*(Section owner: Hardware & Automation Lead)*

Three automation rules are implemented in `evaluateAutomationRules()` in
`firmware/src/main.cpp`, evaluated on every loop iteration in priority order
(R3 > R1 > R2):

| Rule | Condition (IF) | Action (THEN) | Loop type |
|------|-----------------|----------------|-----------|
| R1 – Temperature | Temperature > 30°C (DHT22) | Relay ON → fan runs; turns OFF once temp < 28°C (2°C hysteresis band) | Closed-loop |
| R2 – Lighting | Light level < threshold (LDR) **AND** motion detected (PIR) | LED ON only when dark **and** someone is present | Open-loop |
| R3 – Energy saving | No PIR motion for > 5 minutes | All actuators forced OFF; buzzer gives 1 short beep | Open-loop |

## 5.1 Open-loop vs closed-loop

**R1 (fan) is closed-loop.** The DHT22 continuously feeds temperature readings
back into the same rule that controls the fan, so the fan's own effect on the
room (cooling it down) is measured and used to decide when to switch off again.
A 2°C hysteresis gap (ON at 30°C, OFF at 28°C) prevents the relay from rapidly
chattering when the temperature hovers around a single threshold.

**R2 (light) and R3 (energy saving) are open-loop.** Neither rule measures the
effect of its own action: turning the LED on does not change the LDR reading in
a way the system compensates for, and forcing actuators off after a motion
timeout does not feed back into the PIR sensor. Both simply react to sensor
input using fixed conditions/timers.

## 5.2 Automation code walkthrough

**Sensor read with NaN safety (non-blocking, every 2 s):**

```cpp
void readSensors(unsigned long now) {
  if (now - lastSensorRead < SENSOR_READ_INTERVAL_MS) return;
  lastSensorRead = now;

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    sensorFault = true; // keep last-known values, but flag safe state
  } else {
    sensorFault = false;
    temperature = t;
    humidity = h;
  }

  lightLevel = analogRead(LDR_PIN);
  motionDetected = digitalRead(PIR_PIN) == HIGH;
  if (motionDetected) lastMotionTime = now;
}
```

The DHT22 only supports ~0.5 Hz sampling, so a `millis()`-based interval guard
is used instead of `delay()`, keeping the rest of the loop (PIR/LDR polling,
buzzer timing) fully responsive.

**Rule evaluation (priority order, hysteresis, fail-safe):**

```cpp
void evaluateAutomationRules(unsigned long now) {
  // R3 - highest priority
  bool noMotionTimeout = (now - lastMotionTime) > NO_MOTION_TIMEOUT_MS;
  if (noMotionTimeout) {
    if (!energySavingActive) { energySavingActive = true; triggerBuzzerBeep(now); }
    setFan(false);
    setLed(false);
    return;
  }
  energySavingActive = false;

  // R1 - closed-loop, 2C hysteresis, fail-safe on sensor fault
  if (sensorFault) {
    setFan(false);
  } else if (!fanState && temperature > TEMP_ON_THRESHOLD) {
    setFan(true);
  } else if (fanState && temperature < TEMP_OFF_THRESHOLD) {
    setFan(false);
  }

  // R2 - open-loop
  bool isDark = lightLevel < LDR_DARK_THRESHOLD;
  setLed(isDark && motionDetected);
}
```

**Non-blocking buzzer beep** uses the same `millis()` pattern instead of
`delay()`, so a single short beep does not stall sensor reads or actuator
updates:

```cpp
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
```

An integration hook is left at the end of `loop()` for the Cloud/Comm. lead to
publish `temperature`, `humidity`, `lightLevel`, `motionDetected`, `fanState`
and `ledState` over MQTT to ThingSpeak without needing to touch the sensor or
rule logic.
