# Subsystem 5 — test log

**Subsystem:** Scan coordination, room projection, LCD and full-system integration  ·  **Owner:** Wei En  ·  **Student ID:** <student ID>  ·  **Code:** `pico_b/subsystem5/`

See `testing/README.md` for how to run tests and record results.

## Test cases

Define a test here before running it. Test IDs never change.

| ID | What is tested | Requirement / interface | Method | Steps / how | Expected result |
|---|---|---|---|---|---|
| S5-U01 | display bring-up pattern | | manual | Run `ui.init()` and draw a crosshair (assumption A12); photograph it. | centred crosshair, correct colours, no static |
| S5-U02 | dot-only updates are fast enough | | manual | Time `rect()` for a 2x2 dot and a full-frame fill (assumption A13, `latency_test.py`). | numbers recorded; the UI never redraws a full frame during a scan |
| S5-U03 | one button press = one bearing step | | manual | Press START repeatedly with the sensors facing known targets. | six presses at 15 degrees cover 360 degrees; four dots per press |

*Method:* **auto** = a check in code or a host-side test, result printed on the console;
**manual** = a person observes the result (meter, scope, display, photo…).

## Results

Add a new row for every run. Never edit or delete old rows.

| Date | Test ID | Commit | Tester | Result | Log / evidence | Notes |
|---|---|---|---|---|---|---|
| YYYY-MM-DD | S5-U01 | `abc1234` | | PASS / FAIL | `logs/YYYY-MM-DD_….txt` | |
