"""Subsystem 5: user interface (Pico B): ST7735S map display and buttons."""
import time
from machine import SoftSPI, Pin
import pins_b

_tft = None
_buttons = {}
_last_edge = {}
DEBOUNCE_MS = 30


def init():
    global _tft
    from ST7735 import TFT
    spi = SoftSPI(baudrate=100000, polarity=0, phase=0,
                  sck=Pin(pins_b.TFT_SCK), mosi=Pin(pins_b.TFT_MOSI), miso=Pin(pins_b.TFT_MISO_DUMMY))
    _tft = TFT(spi, pins_b.TFT_DC, pins_b.TFT_RST)
    _tft.init_7735(_tft.REDTAB)      # other tab settings gave wrong colours or an offset image
    time.sleep_ms(300)
    _tft.fill(TFT.BLACK)
    for name, gpio in (("start", pins_b.BTN_START), ("stop", pins_b.BTN_STOP), ("view", pins_b.BTN_VIEW)):
        _buttons[name] = Pin(gpio, Pin.IN, Pin.PULL_UP)
        _last_edge[name] = 0


def pressed(name):
    """True once per press (active low, debounced)."""
    now = time.ticks_ms()
    if _buttons[name].value() == 0 and time.ticks_diff(now, _last_edge[name]) > DEBOUNCE_MS:
        _last_edge[name] = now
        return True
    return False


def draw_point(x, y, colour=None):
    """Draw a 2x2 dot. A full 128x160 frame takes ~3.3 s at 100 kHz, so never redraw the whole screen."""
    from ST7735 import TFT
    _tft.rect((x, y), (2, 2), TFT.WHITE if colour is None else colour)
