# 360° Time-of-Flight 3D Environment Scanner (INF2004, team AAI11)

Two Raspberry Pi Pico WH boards. **Pico A** (FreeRTOS, C/C++) drives a
two-axis BLDC gimbal with AS5600 encoder feedback and a safety monitor.
**Pico B** (MicroPython) reads four Time-of-Flight sensors (two VL53L0X, two
VL53L1X), plots a top-down map on a 128 × 160 ST7735S LCD, and sends motion
commands to Pico A over a wired UART. The design is in the Week 6 design review;
this repo holds the firmware that implements it.

The folder layout (`TEAM.md`, `testing/`, `videos/`) comes from the course
template, [sirfonzie/INF2004_ProjectTemplate](https://github.com/sirfonzie/INF2004_ProjectTemplate).
The template's micro T-Kernel code is not used: the design review specifies
FreeRTOS on Pico A and MicroPython on Pico B.

```
pico_a/                 Pico A firmware (FreeRTOS, C/C++, CMake)
  src/app_main.c, app.h, pins_a.h, FreeRTOSConfig.h
  src/subsystem1/       UART link and frame protocol        (Gabriel)
  src/subsystem2/       motor control, planner, AS5600      (Gerome)
  src/subsystem4/       safety monitor                      (Matthias)
  tests/                host-side tests (no hardware)
pico_b/                 Pico B firmware (MicroPython)
  main.py, pins_b.py
  subsystem1/           UART link and frame protocol        (Gabriel)
  subsystem3/           ToF sensors                         (Kevin)
  subsystem5/           scan coordinator, display, buttons  (Wei En)
  tests/latency_test.py latency measurements for the design review
TEAM.md                 owners, pins, priorities, interfaces
testing/                test plans and results
videos/                 index of unlisted YouTube videos of tests and demos
```

## Status

Honest summary of what exists today.

| Part | State |
|---|---|
| Pico A build | Compiles with Pico SDK 2.2.0 and FreeRTOS-Kernel. **Not yet run on hardware.** |
| UART framing | Written in C and Python; unit-tested on a host, and both encoders produce identical bytes. Not yet tested over a wire. |
| Pico A motion | Skeleton only: encoders are read and the settle check runs, but **there is no motor drive yet** (no FOC or phase duties). |
| Pico A safety | Enables are low at boot; any fault or a lost heartbeat (500 ms) drops them. Fault reporting to Pico B and the reset sequence are TODO. |
| Pico B ToF, display, buttons | Written from the working bench program and the pin map. **Not re-run on hardware after being split into modules.** |
| Pico B scan | Bench behaviour only: one button press is one 15° bearing step. The MOVE / SETTLED handshake and the serpentine schedule are TODO. |
| Latency numbers | `pico_b/tests/latency_test.py` exists; **no results recorded yet**. |

`TODO(Sn owner)` comments in the code mark the work left for each subsystem.

## 1. Pico A (FreeRTOS)

Needs the Arm GNU toolchain (`arm-none-eabi-gcc`), CMake, the Pico SDK and FreeRTOS-Kernel.

```sh
mkdir -p ~/pico && cd ~/pico
git clone --branch 2.2.0 https://github.com/raspberrypi/pico-sdk.git
(cd pico-sdk && git submodule update --init lib/tinyusb)
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git
export PICO_SDK_PATH=~/pico/pico-sdk
export FREERTOS_KERNEL_PATH=~/pico/FreeRTOS-Kernel

cd <this repo>/pico_a
cmake -S . -B build
cmake --build build -j8        # -> build/pico_a_motion.uf2
```

Flash: hold **BOOTSEL**, plug in the Pico, copy `pico_a_motion.uf2` onto the
`RPI-RP2` drive. Logs go to the USB serial port; UART0 (GP0/GP1) is reserved
for the link to Pico B.

Host-side tests (no hardware needed):

```sh
cd pico_a
gcc -Isrc -o /tmp/test_protocol tests/test_protocol.c src/subsystem1/protocol.c && /tmp/test_protocol
```

## 2. Pico B (MicroPython)

1. Install the MicroPython firmware for the Pico W from <https://micropython.org/download/RPI_PICO_W/>
   (hold BOOTSEL, plug in, copy the `.uf2`).
2. Put the third-party drivers the code imports on the board (not vendored here;
   check each licence before redistributing):

   | File on the Pico | Source |
   |---|---|
   | `vl53l0x.py` | <https://github.com/kevinmcaleer/vl53l0x> |
   | `ST7735.py` | <https://github.com/AntonHermann/st7735-pico-micropython> |
   | `vl53l1x.py` | already in `pico_b/subsystem3/` (from <https://github.com/drakxtwo/vl53l1x_pico>, MIT) |

3. Copy the program and run it, using `mpremote` (`pip install mpremote`):

   ```sh
   cd pico_b
   mpremote connect auto fs cp vl53l0x.py ST7735.py :     # after downloading them here
   mpremote connect auto fs cp -r . :
   mpremote connect auto reset                             # main.py runs at power-up
   ```

   Thonny works too: copy the files keeping the folder structure.

4. Latency test (prints per-sensor, four-sensor, display and refresh numbers):

   ```sh
   mpremote connect auto run tests/latency_test.py
   ```

## 3. Wiring

The pin maps are in `TEAM.md` section 2, `pico_a/src/pins_a.h` and `pico_b/pins_b.py`.
Link between boards: A GP0 (TX) to B GP17 (RX), A GP1 (RX) to B GP16 (TX), and a common GND.

## 4. Working as a team

- Only the owner edits a subsystem's folders (`TEAM.md`, section 1).
- Claim pins, priorities and interfaces in `TEAM.md` before you wire or code them.
- Record every test in `testing/` with the commit it ran on; share videos as
  **unlisted YouTube** links listed in `videos/README.md`. Never commit video files.
- Never commit secrets or build output (`.gitignore` covers `build/`).
