# Subsystem 3 — test log

**Subsystem:** ToF sensing (initialisation, calibration, ranging, quality flags)  ·  **Owner:** Kevin  ·  **Student ID:** <student ID>  ·  **Code:** `pico_b/subsystem3/`

See `testing/README.md` for how to run tests and record results.

## Test cases

Define a test here before running it. Test IDs never change.

| ID | What is tested | Requirement / interface | Method | Steps / how | Expected result |
|---|---|---|---|---|---|
| S3-U01 | `i2c.scan()` on each of the four buses shows exactly 0x29 | | manual | Run `i2c.scan()` on each bus (assumption A4). | one device at 0x29 per bus |
| S3-U02 | model ID of each module at boot | | manual | Run `tof.init()` and read the model IDs (assumption A11); photograph the part markings. | two VL53L0X (0xEE) and two VL53L1X (0xEACC) |
| S3-U03 | four-sensor read time | | manual | Run `pico_b/tests/latency_test.py`; save the console output. | numbers recorded; four-sensor rate noted |
| S3-U04 | stationary repeatability | | manual | 21 consecutive reads at about 200 ms spacing on a fixed target. | spread recorded per sensor |

*Method:* **auto** = a check in code or a host-side test, result printed on the console;
**manual** = a person observes the result (meter, scope, display, photo…).

## Results

Add a new row for every run. Never edit or delete old rows.

| Date | Test ID | Commit | Tester | Result | Log / evidence | Notes |
|---|---|---|---|---|---|---|
| YYYY-MM-DD | S3-U01 | `abc1234` | | PASS / FAIL | `logs/YYYY-MM-DD_….txt` | |
