"""Subsystem 5: scan coordinator (Pico B). Stepwise state machine; call step() every loop pass.

Bench behaviour: one button press = one bearing step. Full system: MOVE to Pico A, wait for
SETTLED with the matching seq, then capture. TODO(S5 owner): the serpentine schedule (Appendix A.2)."""
import math
from subsystem1 import link
from subsystem3 import tof
from subsystem5 import ui

IDLE, MOVING, CAPTURING = range(3)
CX, CY = 64, 80            # map centre on the 128 x 160 panel
MAX_RANGE_MM = 2000        # 2 m radius -> 64 px (31 mm per pixel)
SENSOR_OFFSETS_DEG = (0, 90, 180, 270)   # four sensors at 90 degrees
STEP_DEG = 15              # 24 bearings over 360 degrees in six presses

state = IDLE
bearing_deg = 0
points = []                # (angle_deg, mm) of every valid reading


def _plot(angle_deg, mm):
    r = mm * 64 // MAX_RANGE_MM
    a = math.radians(angle_deg)
    ui.draw_point(CX + int(r * math.sin(a)), CY - int(r * math.cos(a)))


def step():
    global state, bearing_deg
    if state == IDLE and ui.pressed("start"):
        state = CAPTURING            # bench: no motor yet, so skip MOVING
    if state == CAPTURING:
        for off, (mm, valid) in zip(SENSOR_OFFSETS_DEG, tof.capture()):
            if valid:
                angle = (bearing_deg + off) % 360
                points.append((angle, mm))
                _plot(angle, mm)
        bearing_deg = (bearing_deg + STEP_DEG) % 360
        state = IDLE
    if ui.pressed("stop"):
        link.send_stop()
        state = IDLE
