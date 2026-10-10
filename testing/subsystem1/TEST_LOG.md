# Subsystem 1 — test log

**Subsystem:** Pico-to-Pico UART link (protocol, framing, acknowledgements, logging)  ·  **Owner:** Gabriel  ·  **Student ID:** <student ID>  ·  **Code:** `pico_a/src/subsystem1/` and `pico_b/subsystem1/`

See `testing/README.md` for how to run tests and record results.

## Test cases

Define a test here before running it. Test IDs never change.

| ID | What is tested | Requirement / interface | Method | Steps / how | Expected result |
|---|---|---|---|---|---|
| S1-U01 | CRC-16/CCITT-FALSE matches the known value for `123456789` | | auto | `pico_a/tests/test_protocol.c` | `[TEST] S1-U01 PASS` |
| S1-U02 | a MOVE frame encodes and decodes unchanged | | auto | `pico_a/tests/test_protocol.c` | `[TEST] S1-U02 PASS` |
| S1-U03 | a frame with one corrupted byte is rejected | | auto | `pico_a/tests/test_protocol.c` | `[TEST] S1-U03 PASS` |
| S1-U04 | the parser resynchronises after garbage bytes | | auto | `pico_a/tests/test_protocol.c` | `[TEST] S1-U04 PASS` |
| S1-U05 | C and Python encoders produce identical bytes | | auto | `pico_a/tests/test_protocol.c` (expected bytes come from `protocol.py`) | `[TEST] S1-U05 PASS` |
| S1-U06 | heartbeats cross the wire in both directions | | manual | 1. Wire A GP0/GP1 to B GP17/GP16 (TX to RX) and join GND. 2. Flash both. 3. Watch `link.alive()` on B and the heartbeat timestamp on A. | both sides see a heartbeat at about 100 ms intervals |

*Method:* **auto** = a check in code or a host-side test, result printed on the console;
**manual** = a person observes the result (meter, scope, display, photo…).

## Results

Add a new row for every run. Never edit or delete old rows.

| Date | Test ID | Commit | Tester | Result | Log / evidence | Notes |
|---|---|---|---|---|---|---|
| YYYY-MM-DD | S1-U01 | `abc1234` | | PASS / FAIL | `logs/YYYY-MM-DD_….txt` | |
