"""Pico WH B: cooperative main loop (MicroPython, no RTOS).
The Pico B 'tasks' in the design review are the loop stages below; priority means call order."""
import time
from subsystem1 import link
from subsystem3 import tof
from subsystem5 import ui, scan

tof.init()
ui.init()

while True:
    link.poll()      # UART Link          (priority 5)
    scan.step()      # Scan Coordinator   (priority 4); calls tof.capture() (priority 3) and ui (priority 2)
    time.sleep_ms(5)
