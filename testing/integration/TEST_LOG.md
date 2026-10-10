# Integration — test log

Tests that two or more subsystems work together. There is one test (or more) for every interface listed in TEAM.md section 5. Both owners agree on the test and sign off the result.

See `testing/README.md` for how to run tests and record results.

## Test cases

Define a test here before running it. Test IDs never change.

| ID | What is tested | Requirement / interface | Method | Steps / how | Expected result |
|---|---|---|---|---|---|
| INT-01 | B sends MOVE, A answers SETTLED with the same seq | IF-01, IF-02 | manual | 1. Both Picos running, UART wired. 2. Send MOVE from B. 3. Log the SETTLED frame. | SETTLED seq equals MOVE seq; pose within tolerance |
| INT-02 | heartbeat loss on either side raises a fault | IF-03 | manual | 1. Run both Picos. 2. Unplug the UART. | A drops EN within 500 ms; B shows an error and cancels the scan |

*Method:* **auto** = a check in code or a host-side test, result printed on the console;
**manual** = a person observes the result (LED, scope, meter, phone app…).

## Results

Add a new row for every run. Never edit or delete old rows.

| Date | Test ID | Commit | Tester | Result | Log / evidence | Notes |
|---|---|---|---|---|---|---|
| YYYY-MM-DD | INT-01 | `abc1234` | | PASS / FAIL | `logs/YYYY-MM-DD_….txt` | |
