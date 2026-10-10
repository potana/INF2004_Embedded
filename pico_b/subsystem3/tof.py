"""Subsystem 3: ToF acquisition (Pico B). Owns the four sensor buses.

Each sensor sits alone on its own SoftI2C bus at 0x29. Two are VL53L0X (driver
call .ping()) and two are VL53L1X (driver call .read()). Drivers needed on the
Pico besides vl53l1x.py (in this folder): vl53l0x.py (see ../README.md)."""
from machine import SoftI2C, Pin
import pins_b

OUT_OF_RANGE_MM = 8190   # both drivers return about 8190 when nothing is in range

_sensors = []


def init():
    """Create the four drivers. Raises if a module is missing or the wrong chip."""
    from vl53l0x import VL53L0X
    from subsystem3.vl53l1x import VL53L1X
    for sda, scl, chip in pins_b.TOF_BUSES:
        bus = SoftI2C(sda=Pin(sda), scl=Pin(scl))
        _sensors.append(VL53L0X(bus) if chip == "VL53L0X" else VL53L1X(bus))


def capture():
    """Read all four sensors one after another.
    Returns four (distance_mm, valid) tuples. Out-of-range is flagged invalid, never dropped (FR13)."""
    out = []
    for s in _sensors:
        try:
            mm = s.ping() if hasattr(s, "ping") else s.read()
        except OSError:
            out.append((0, False))
            continue
        out.append((mm, 0 < mm < OUT_OF_RANGE_MM))
    return out
