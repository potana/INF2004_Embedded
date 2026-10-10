from machine import SoftSPI, SoftI2C, Pin
from ST7735 import TFT
from vl53l0x import VL53L0X
try:
    from subsystem3.vl53l1x import VL53L1X   # repo layout
except ImportError:
    from vl53l1x import VL53L1X              # driver copied to the Pico root
import time

# ---- Same pins as the working bench program ----
PIN_SCK, PIN_MOSI, PIN_DC, PIN_RST = 9, 8, 3, 2

spi = SoftSPI(baudrate=100000, polarity=0, phase=0,
              sck=Pin(PIN_SCK), mosi=Pin(PIN_MOSI), miso=Pin(28))
tft = TFT(spi, PIN_DC, PIN_RST)
tft.init_7735(tft.REDTAB)
time.sleep_ms(300)
tft.fill(TFT.BLACK)

i2c_1 = SoftI2C(sda=Pin(0), scl=Pin(1))    # VL53L0X
i2c_2 = SoftI2C(sda=Pin(4), scl=Pin(5))    # VL53L1X
i2c_3 = SoftI2C(sda=Pin(6), scl=Pin(7))    # VL53L0X
i2c_4 = SoftI2C(sda=Pin(26), scl=Pin(27))  # VL53L1X

s1 = VL53L0X(i2c_1)
s2 = VL53L1X(i2c_2)
s3 = VL53L0X(i2c_3)
s4 = VL53L1X(i2c_4)

N = 100  # iterations per test

def stats(name, samples):
    lo = min(samples)
    hi = max(samples)
    mean = sum(samples) / len(samples)
    print("%-22s min %7.2f  mean %7.2f  max %7.2f ms" % (name, lo / 1000, mean / 1000, hi / 1000))
    return mean

def time_call(fn, n=N):
    out = []
    for _ in range(n):
        t0 = time.ticks_us()
        fn()
        out.append(time.ticks_diff(time.ticks_us(), t0))
    return out

# warm-up so first-call setup is not counted
for _ in range(5):
    s1.ping(); s2.read(); s3.ping(); s4.read()

print("=== Per-sensor read time (N=%d) ===" % N)
m1 = stats("S1 VL53L0X  ping()", time_call(s1.ping))
m2 = stats("S2 VL53L1X  read()", time_call(s2.read))
m3 = stats("S3 VL53L0X  ping()", time_call(s3.ping))
m4 = stats("S4 VL53L1X  read()", time_call(s4.read))

def four():
    s1.ping(); s2.read(); s3.ping(); s4.read()

print("\n=== Four-sensor read, back to back (N=%d) ===" % N)
mt = stats("All four, sequential", time_call(four))
print("Sum of per-sensor means: %.2f ms" % ((m1 + m2 + m3 + m4) / 1000))
print("Four-sensor rate:        %.1f Hz" % (1000000 / (mt)))

# One full bearing step as the bench does it: 4 reads + 4 plot points
def plot_pt():
    tft.rect((64, 80), (2, 2), TFT.WHITE)

def step():
    four()
    for _ in range(4):
        plot_pt()

print("\n=== Display ===")
stats("One 2x2 rect()", time_call(plot_pt, 20))
stats("Reads + 4 rect()", time_call(step, 20))

t0 = time.ticks_us()
tft.fill(TFT.BLACK)
t_fill = time.ticks_diff(time.ticks_us(), t0)
print("Full-frame fill:       %.0f ms (SoftSPI @ 100 kHz)" % (t_fill / 1000))

# Distinct-value rate: how often does each sensor return a NEW value?
# (Shows measurement refresh, not just bus transfer time.)
print("\n=== Fresh-value rate over 3 s (distinct consecutive readings) ===")
def fresh(fn):
    last = None
    changes = 0
    end = time.ticks_add(time.ticks_ms(), 3000)
    while time.ticks_diff(end, time.ticks_ms()) > 0:
        v = fn()
        if v != last:
            changes += 1
            last = v
    return changes / 3.0

print("S1 changes/s: %.1f" % fresh(s1.ping))
print("S2 changes/s: %.1f" % fresh(s2.read))
print("S3 changes/s: %.1f" % fresh(s3.ping))
print("S4 changes/s: %.1f" % fresh(s4.read))

print("\nDone. Point all four sensors at a wall ~300 mm away and keep them still for a clean result.")
