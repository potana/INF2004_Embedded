"""Pico-to-Pico UART frame. PROPOSED: the subsystem 1 owner finalises it.
Mirror of pico_a/src/subsystem1/protocol.h. Keep both identical.

    [0xA5][type][seq][len][payload x len][crc_hi][crc_lo]

CRC is CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF) over type..payload.
Angles are int16 centi-degrees, big-endian."""
import struct

SOF = 0xA5
MAX_PAYLOAD = 8

MOVE, SETTLED, STOP, FAULT, HEARTBEAT, ACK = 1, 2, 3, 4, 5, 6


def crc16(data):
    crc = 0xFFFF
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) & 0xFFFF if crc & 0x8000 else (crc << 1) & 0xFFFF
    return crc


def encode(msg_type, seq, payload=b""):
    if len(payload) > MAX_PAYLOAD:
        raise ValueError("payload too long")
    body = bytes((msg_type, seq & 0xFF, len(payload))) + payload
    return bytes((SOF,)) + body + struct.pack(">H", crc16(body))


def move(seq, tilt_cdeg, sweep_cdeg):
    return encode(MOVE, seq, struct.pack(">hh", tilt_cdeg, sweep_cdeg))


class Parser:
    """Feed bytes one at a time; push() returns (type, seq, payload) when a good frame completes."""

    def __init__(self):
        self.buf = bytearray()

    def push(self, byte):
        b = self.buf
        if not b:
            if byte == SOF:
                b.append(byte)
            return None
        b.append(byte)
        if len(b) == 4 and b[3] > MAX_PAYLOAD:
            del b[:]
            return None
        if len(b) >= 4 and len(b) == 6 + b[3]:
            n = b[3]
            frame = bytes(b)
            del b[:]
            if crc16(frame[1:4 + n]) != (frame[4 + n] << 8 | frame[5 + n]):
                return None
            return frame[1], frame[2], frame[4:4 + n]
        return None
