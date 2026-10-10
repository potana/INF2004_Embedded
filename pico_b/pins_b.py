"""Pico WH B pin map. Source of truth: design review, Section 2.2.
Keep this file and TEAM.md (pin table) identical."""

# Four ToF sensors, one software-I2C bus each, all at the default address 0x29.
# Two modules are VL53L0X and two are VL53L1X; each family has its own driver.
TOF_BUSES = (
    # (sda, scl, chip)
    (0, 1, "VL53L0X"),
    (4, 5, "VL53L1X"),
    (6, 7, "VL53L0X"),
    (26, 27, "VL53L1X"),
)

# ST7735S 128x160 TFT, software SPI at 100 kHz (these GPIOs are not hardware-SPI pins).
TFT_SCK, TFT_MOSI = 9, 8
TFT_DC, TFT_RST = 3, 2
TFT_MISO_DUMMY = 28      # the SoftSPI constructor needs a MISO pin; it is never used
# TFT CS is tied to GND and BLK is not connected: no GPIO.

# Buttons (active low, internal pull-up).
BTN_START = 20           # START / one scan step per press
BTN_STOP = 10            # proposed: cancel scan (not yet on the bench)
BTN_VIEW = 11            # proposed: cycle views / acknowledge (not yet on the bench)

# UART0 to Pico A: B.TX(GP16) -> A.RX(GP1), B.RX(GP17) <- A.TX(GP0), common GND.
LINK_UART_ID = 0
LINK_TX, LINK_RX = 16, 17
LINK_BAUD = 115200
