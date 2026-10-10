# Subsystem 2 — test log

**Subsystem:** Gimbal motion (BLDC driver, AS5600 feedback, closed-loop motion, cable management)  ·  **Owner:** Gerome  ·  **Student ID:** <student ID>  ·  **Code:** `pico_a/src/subsystem2/`

See `testing/README.md` for how to run tests and record results.

## Test cases

Define a test here before running it. Test IDs never change.

| ID | What is tested | Requirement / interface | Method | Steps / how | Expected result |
|---|---|---|---|---|---|
| S2-U01 | both AS5600 encoders return a raw angle | | manual | Read the raw angle on each I2C bus while turning the shaft by hand (assumption A3). | values change smoothly over 0 to 4095, no I2C errors |
| S2-U02 | settle detection | | manual | Command a setpoint with the motors enabled; watch `g_settled`. | `g_settled` goes true only after the pose is within 1.0 degree for 50 ms |
| S2-U03 | gimbal clears cables and casing | | manual | Unpowered manual sweep over the full range (assumption A2). | no snagging; travel stops fitted |

*Method:* **auto** = a check in code or a host-side test, result printed on the console;
**manual** = a person observes the result (meter, scope, display, photo…).

## Results

Add a new row for every run. Never edit or delete old rows.

| Date | Test ID | Commit | Tester | Result | Log / evidence | Notes |
|---|---|---|---|---|---|---|
| YYYY-MM-DD | S2-U01 | `abc1234` | | PASS / FAIL | `logs/YYYY-MM-DD_….txt` | |
