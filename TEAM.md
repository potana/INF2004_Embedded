# Team and subsystems

Project: **360° Time-of-Flight 3D Environment Scanner** (team AAI11). The
allocation below follows the design review (Section "Team Role and Subsystem
Ownership"). Keep this file up to date: it is the single place that says
**who owns what**, so five people can work in two firmware images without
clashing over pins, priorities or interfaces.

Rule: **only the owner edits a subsystem's folders.** To change another
subsystem, ask its owner or open a pull request for them to review.

Two boards:

| Board | Role | Firmware |
|---|---|---|
| **Pico WH A** | motion controller: two BLDC motors, two AS5600 encoders, safety | FreeRTOS, C/C++ (`pico_a/`) |
| **Pico WH B** | perception and interface: four ToF sensors, LCD, buttons | MicroPython, one cooperative loop, no RTOS (`pico_b/`) |

## 1. Members and subsystems

| # | Owner | Subsystem | Student ID | GitHub username | Code | Tests |
|---|---|---|---|---|---|---|
| 1 | Gabriel | Pico-to-Pico UART protocol, framing, acknowledgements, logging | | | `pico_a/src/subsystem1/`, `pico_b/subsystem1/` | `testing/subsystem1/` |
| 2 | Gerome | BLDC driver, AS5600 feedback, closed-loop motion, cable management | | | `pico_a/src/subsystem2/` | `testing/subsystem2/` |
| 3 | Kevin | ToF initialisation, calibration, ranging, quality flags | | | `pico_b/subsystem3/` | `testing/subsystem3/` |
| 4 | Matthias | Safety monitor, power checks, mechanical limits, verification tests | | | `pico_a/src/subsystem4/` | `testing/subsystem4/` |
| 5 | Wei En | Scan coordination, room projection, LCD, full-system integration | | | `pico_b/subsystem5/` | `testing/subsystem5/` |

Shared files (`pico_a/src/app_main.c`, `pico_a/src/app.h`, `pico_b/main.py`,
the two pin files, this file) are changed only by agreement of the team. The
exception is claiming an empty row in this file (a pin, a peripheral, an
interface ID), which anyone may do.

## 2. Pins

One owner per pin. Claim a pin here **before** you wire or code it. The
pin files `pico_a/src/pins_a.h` and `pico_b/pins_b.py` must match these tables.

Never use GP23, GP24, GP25, GP29 on a Pico WH: they are wired to the radio.

### Pico WH A

| Pin | Owner | Used for |
|---|---|---|
| GP0 | S1 | UART0 TX to Pico B (B GP17) |
| GP1 | S1 | UART0 RX from Pico B (B GP16) |
| GP4 | S2 | AS5600 encoder 1 SDA (I2C0, 0x36) |
| GP5 | S2 | AS5600 encoder 1 SCL |
| GP6 | S2 | AS5600 encoder 2 SDA (I2C1, 0x36) |
| GP7 | S2 | AS5600 encoder 2 SCL |
| GP10, GP11, GP12 | S2 | Motor 1 IN1–IN3 (3× PWM) |
| GP13, GP14, GP15 | S2 | Motor 2 IN4–IN6 (3× PWM); GP12 and GP13 share a PWM slice, so one PWM frequency for both motors |
| GP16 | S4 | Motor 1 enable (low during boot and on fault) |
| GP17 | S4 | Motor 2 enable (low during boot and on fault) |
| GP18 | | free; suggested for the driver FAULT output |
| GP2, GP3, GP8, GP9, GP19–GP22, GP26–GP28 | | free |

Driver RESET / SLEEP / FAULT are not assigned yet. Motor supply goes to the
driver supply input only, never to a Pico pin.

### Pico WH B

| Pin | Owner | Used for |
|---|---|---|
| GP0, GP1 | S3 | ToF 1 (VL53L0X) SDA, SCL, software I2C bus 1 |
| GP4, GP5 | S3 | ToF 2 (VL53L1X) SDA, SCL, bus 2 |
| GP6, GP7 | S3 | ToF 3 (VL53L0X) SDA, SCL, bus 3 |
| GP26, GP27 | S3 | ToF 4 (VL53L1X) SDA, SCL, bus 4 |
| GP9, GP8 | S5 | TFT SCK, MOSI (software SPI, 100 kHz) |
| GP3 | S5 | TFT D/C |
| GP2 | S5 | TFT RESET |
| GP28 | S5 | dummy MISO for the SoftSPI constructor (not wired) |
| GP20 | S5 | Button 1: START / one scan step per press |
| GP10 | S5 | Button 2: STOP (proposed, not yet on the bench) |
| GP11 | S5 | Button 3: VIEW / ACK (proposed, not yet on the bench) |
| GP16 | S1 | UART0 TX to Pico A (A GP1) |
| GP17 | S1 | UART0 RX from Pico A (A GP0) |
| GP12–GP15, GP18, GP19, GP21, GP22 | | free |

TFT CS is tied to GND and BLK is not connected, so neither uses a GPIO.

## 3. Peripherals

**Subsystems, tasks and peripherals are three different things:** a subsystem
is one student's part of the product; a task is one thread of execution (on
Pico B, one stage of the main loop); a peripheral is one hardware block.

**The rule: each peripheral is driven by exactly one task, its *owning task*.**
Every other task asks the owning task through a queue and never touches that
peripheral itself.

| Board | Peripheral | Owner subsystem | Owning task | Pins |
|---|---|---|---|---|
| A | UART0 | S1 | Link Task | GP0, GP1 |
| A | I2C0 | S2 | Motor Control Task | GP4, GP5 |
| A | I2C1 | S2 | Motor Control Task | GP6, GP7 |
| A | PWM (6 outputs; slices shared by GP12/GP13) | S2 | Motor Control Task | GP10–GP15 |
| A | Motor enables (GPIO) | S4 | Safety Monitor Task | GP16, GP17 |
| B | UART0 | S1 | Link (loop stage) | GP16, GP17 |
| B | 4 × software I2C | S3 | ToF Acquisition (loop stage) | GP0/1, GP4/5, GP6/7, GP26/27 |
| B | software SPI | S5 | User Interface (loop stage) | GP9, GP8 (+GP3, GP2) |
| B | Buttons (GPIO) | S5 | User Interface (loop stage) | GP20, GP10, GP11 |

## 4. Task priorities

**Pico A (FreeRTOS: a larger number is a higher priority).** Defined in `pico_a/src/app.h`.

| Priority | Task | Subsystem | Period / trigger |
|---|---|---|---|
| 6 | Safety Monitor | S4 | 10 ms periodic + fault events |
| 5 | Motor Control | S2 | fixed high-rate loop (5 ms in the skeleton, rate TBD) |
| 4 | UART Link | S1 | UART RX + 100 ms heartbeat |
| 3 | Motion Planner | S2 | on each MOVE command |

**Pico B (MicroPython, no RTOS).** "Priority" is call order in `pico_b/main.py`.

| Priority | Loop stage | Subsystem |
|---|---|---|
| 5 | Link (`subsystem1/link.py`) | S1 |
| 4 | Scan Coordinator (`subsystem5/scan.py`) | S5 |
| 3 | ToF Acquisition (`subsystem3/tof.py`), called by the coordinator after SETTLED | S3 |
| 2 | User Interface (`subsystem5/ui.py`) | S5 |
| 1 | Processing (filter, register, downsample) | S5, not written yet |

## 5. Interfaces between subsystems

Every arrow between two subsystems is listed here. Agree on the data format
before coding. Each interface is tested by at least one integration test
(`testing/integration/TEST_LOG.md`).

**UART frame (proposed, S1 owner finalises it):**
`[0xA5][type][seq][len][payload][crc_hi][crc_lo]`, CRC-16/CCITT-FALSE over
`type..payload`, angles are int16 centi-degrees, big-endian. Defined twice and
kept identical: `pico_a/src/subsystem1/protocol.h` and `pico_b/subsystem1/protocol.py`.

| ID | From → To | Mechanism | Name | Data / meaning | Agreed by |
|---|---|---|---|---|---|
| IF-01 | B (S5) → A (S1 → S2) | UART `MOVE` (B → A), then `q_move` | `q_move` (`move_cmd_t`) | seq, tilt target, sweep target; accepted only in safe ACTIVE mode | |
| IF-02 | A (S2 → S1) → B (S5) | UART `SETTLED` | `g_settled`, `g_pose` | seq, measured tilt and sweep, settled flag; must match the outstanding MOVE | |
| IF-03 | both ways | UART `HEARTBEAT` | `g_last_heartbeat_ms` | about 100 ms period, 500 ms timeout | |
| IF-04 | B (S5) → A (S1 → S4) | UART `STOP`, then fault bit | `eg_faults` (`FAULT_STOP_CMD`) | seq, reason; A latches the stop and returns `ACK` | |
| IF-05 | A (S4) → B (S1) | UART `FAULT` | | fault ID, measured pose; B cancels the scan and shows the error | |
| IF-06 | S2 → S4 | event group | `eg_faults` | `FAULT_ENCODER_*`, `FAULT_LIMIT` | |
| IF-07 | S4 → S2 | function call | `safety_motors_enabled()` | the motor task drives outputs only while true | |
| IF-08 | S3 → S5 (Pico B) | function call | `tof.capture()` | four `(distance_mm, valid)` tuples, out-of-range flagged invalid | |
| IF-09 | S1 → S5 (Pico B) | function call | `link.send_move()`, `link.take()` | MOVE out; SETTLED / FAULT / ACK in | |

## 6. FreeRTOS objects (Pico A)

Heap: `configTOTAL_HEAP_SIZE` in `pico_a/src/FreeRTOSConfig.h` (64 KB, heap_4).
Current usage in the skeleton:

| Object | Count | Owner |
|---|---|---|
| Task | 4 | S4 safety, S2 motor, S1 link, S2 planner |
| Queue | 2 (`q_move`, depth 4; `q_setpoint`, depth 1, overwrite) | S1 / S2 |
| Event group | 1 (`eg_faults`) | S4 |
