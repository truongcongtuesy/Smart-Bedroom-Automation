# Edge Intelligence

Rule-based occupancy-pattern learning that runs entirely on the ESP32.

**Code location:** PlatformIO only compiles `firmware/src/`, so the module lives there:

- `firmware/src/edge_intelligence.h` / `edge_intelligence.cpp` — the module
- `firmware/src/main.cpp` — five small hooks: `edgeBegin()` in `setup()`, `edgeUpdate()` after each PIR sample,
  `edgeSensorIntervalMs()` as the polling interval, and `edgeKeepLit()` inside rule R2.

## How it works

1. Every PIR sample is counted in one of 24 hourly buckets (samples, and samples with motion).
2. When the current hour has at least `EDGE_MIN_SAMPLES` (20) samples, its motion ratio gives the state:

| State | Condition | Effect |
|---|---|---|
| HIGH | ratio > 70 % | sensors polled every 2 s (DHT22 minimum); light stays on 30 s after the last motion |
| NORMAL | 20 % to 70 % | default, polling every 2.5 s |
| LOW | ratio < 20 % | polling every 5 s to save power |
| LEARNING | fewer than 20 samples | default behaviour |

3. The hour comes from SNTP (Brisbane time, `AEST-10`). If the clock is not synchronised the module falls back to
   hours of uptime.

Log lines look like `[EDGE] hour 14: HIGH (31/40 motion samples = 77%)` and appear only when the hour or state changes.

## Limitations

- Counters live in RAM, so the learned pattern is lost on reset.
- A PIR sensor reports motion, not presence: someone sitting still lowers the ratio.
- Counts accumulate without decay, so old days weigh as much as recent ones.

## Demonstration build (accelerated clock)

A real day cannot be waited out, so a demo build makes one "hour" last 30 s and trusts a ratio after 5 samples.
In a PlatformIO terminal (the setting only lasts for that terminal):

```
$env:PLATFORMIO_BUILD_FLAGS = "-DEDGE_DEMO_HOUR_MS=30000 -DEDGE_MIN_SAMPLES=5"
pio run
```

Start the simulation, press "Simulate motion" on the PIR every few seconds for the first 30 s (hour 00 becomes HIGH),
then leave it alone for the next 30 s (hour 01 becomes LOW). Return to the normal build with
`Remove-Item Env:PLATFORMIO_BUILD_FLAGS` and `pio run`.
