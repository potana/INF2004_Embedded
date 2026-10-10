"""Subsystem 1: UART Link (Pico B). Owns UART0. Call poll() from the main loop."""
import time
from machine import UART, Pin
import pins_b
from subsystem1 import protocol

HEARTBEAT_MS = 100       # design review, Section 4
TIMEOUT_MS = 500

_uart = UART(pins_b.LINK_UART_ID, baudrate=pins_b.LINK_BAUD,
             tx=Pin(pins_b.LINK_TX), rx=Pin(pins_b.LINK_RX))
_parser = protocol.Parser()
_seq = 0
_last_hb_tx = time.ticks_ms()
_last_hb_rx = None
_inbox = []              # decoded (type, seq, payload) frames not yet consumed


def send_move(tilt_cdeg, sweep_cdeg):
    """Send MOVE and return its sequence number."""
    global _seq
    _seq = (_seq + 1) & 0xFF
    _uart.write(protocol.move(_seq, tilt_cdeg, sweep_cdeg))
    return _seq


def send_stop(reason=0):
    global _seq
    _seq = (_seq + 1) & 0xFF
    _uart.write(protocol.encode(protocol.STOP, _seq, bytes((reason,))))
    return _seq


def poll():
    """Read pending bytes, send the heartbeat when due. Never blocks."""
    global _last_hb_tx, _last_hb_rx, _seq
    data = _uart.read()
    if data:
        for byte in data:
            frame = _parser.push(byte)
            if frame is None:
                continue
            if frame[0] == protocol.HEARTBEAT:
                _last_hb_rx = time.ticks_ms()
            else:
                _inbox.append(frame)
    if time.ticks_diff(time.ticks_ms(), _last_hb_tx) >= HEARTBEAT_MS:
        _seq = (_seq + 1) & 0xFF
        _uart.write(protocol.encode(protocol.HEARTBEAT, _seq, b"\x01"))
        _last_hb_tx = time.ticks_ms()


def take():
    """Return the oldest SETTLED / FAULT / ACK frame, or None."""
    return _inbox.pop(0) if _inbox else None


def alive():
    return _last_hb_rx is not None and time.ticks_diff(time.ticks_ms(), _last_hb_rx) < TIMEOUT_MS
