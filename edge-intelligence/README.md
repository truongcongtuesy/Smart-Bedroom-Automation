# Edge Intelligence

**Owner:** You (Integration)

Rule-based occupancy-pattern intelligence: tracks PIR motion by time-of-day
to identify "high-occupancy" windows, then adjusts polling frequency and
lighting readiness accordingly, instead of relying on fixed thresholds
alone.

## Contents

- `occupancy_pattern.ino` / `.cpp` — hourly motion-count tracking +
  high-occupancy threshold logic
- `design_notes.md` — logic design (threshold %, polling rate change, etc.)

## Design Summary

1. Maintain a count of PIR = HIGH events per hour-of-day bucket.
2. Flag an hour as "high-occupancy" once its motion-detection rate exceeds
   a set threshold (e.g. 70% of checks in that hour).
3. During high-occupancy hours: increase sensor polling rate and keep
   lighting logic (R2) more responsive.
4. During low-occupancy hours: reduce polling rate to save power, matching
   the "variable polling" concept referenced in the pitch document.

## Status

- [ ] Design finalised
- [ ] Standalone code implemented and tested
- [ ] Integrated with `firmware/` in Week 9
