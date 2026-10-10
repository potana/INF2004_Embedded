# Subsystem 4 — test log

**Subsystem:** Safety and verification (safety monitor, power checks, mechanical limits, tests)  ·  **Owner:** Matthias  ·  **Student ID:** <student ID>  ·  **Code:** `pico_a/src/subsystem4/`

See `testing/README.md` for how to run tests and record results.

## Test cases

Define a test here before running it. Test IDs never change.

| ID | What is tested | Requirement / interface | Method | Steps / how | Expected result |
|---|---|---|---|---|---|
| S4-U01 | motor enables are low at boot | | manual | Meter on EN1 and EN2 while resetting Pico A. | both low until the safety task enables them |
| S4-U02 | heartbeat loss drops the enables | | manual | Unplug the UART from B while the motors are enabled. | EN1 and EN2 low within 500 ms (plus 10 ms task period) |
| S4-U03 | STOP command latches a stop | | manual | Send STOP from B while enabled. | enables dropped, ACK returned |
| S4-U04 | motor supply and logic supply within ratings | | manual | Current-limited bench supply; inspect the adapter rating (assumptions A6, A7). | no brown-out; Pico supply within its rating |

*Method:* **auto** = a check in code or a host-side test, result printed on the console;
**manual** = a person observes the result (meter, scope, display, photo…).

## Results

Add a new row for every run. Never edit or delete old rows.

| Date | Test ID | Commit | Tester | Result | Log / evidence | Notes |
|---|---|---|---|---|---|---|
| YYYY-MM-DD | S4-U01 | `abc1234` | | PASS / FAIL | `logs/YYYY-MM-DD_….txt` | |
