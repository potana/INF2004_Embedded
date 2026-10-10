/* S1-U01 / S1-U02: host-side test of the UART framing (no hardware).
 *   gcc -Isrc -o /tmp/test_protocol tests/test_protocol.c src/subsystem1/protocol.c && /tmp/test_protocol */
#include <stdio.h>
#include <string.h>
#include "subsystem1/protocol.h"

static int failures;
#define CHECK(id, cond) do { printf("[TEST] %s %s\n", id, (cond) ? "PASS" : "FAIL"); if (!(cond)) failures++; } while (0)

int main(void)
{
    /* Known CRC-16/CCITT-FALSE value for "123456789". */
    CHECK("S1-U01", crc16_ccitt((const uint8_t *)"123456789", 9) == 0x29B1);

    frame_t in = { .type = MSG_MOVE, .seq = 7, .len = 4 };
    put_i16(&in.payload[0], -1234); put_i16(&in.payload[2], 4500);
    uint8_t wire[FRAME_MAX_LEN];
    size_t n = frame_encode(&in, wire);
    /* The same MOVE built by pico_b/subsystem1/protocol.py: a5010704fb2e1194e702 */
    static const uint8_t expect[] = { 0xa5,0x01,0x07,0x04,0xfb,0x2e,0x11,0x94,0xe7,0x02 };
    CHECK("S1-U05", n == sizeof expect && memcmp(wire, expect, n) == 0);
    frame_parser_t p; frame_parser_init(&p);
    frame_t out; bool got = false;
    for (size_t i = 0; i < n; i++) got = frame_parser_push(&p, wire[i], &out);
    CHECK("S1-U02", got && out.type == MSG_MOVE && out.seq == 7 && out.len == 4 &&
                    get_i16(&out.payload[0]) == -1234 && get_i16(&out.payload[2]) == 4500);

    /* Corrupt one payload byte: the frame must be rejected. */
    wire[5] ^= 0x40; frame_parser_init(&p); got = false;
    for (size_t i = 0; i < n; i++) got |= frame_parser_push(&p, wire[i], &out);
    CHECK("S1-U03", !got);

    /* Garbage before a good frame: the parser must resynchronise. */
    wire[5] ^= 0x40; frame_parser_init(&p); got = false;
    uint8_t junk[] = { 0x00, 0xFF, 0x13 };
    for (size_t i = 0; i < sizeof junk; i++) frame_parser_push(&p, junk[i], &out);
    for (size_t i = 0; i < n; i++) got |= frame_parser_push(&p, wire[i], &out);
    CHECK("S1-U04", got);

    return failures ? 1 : 0;
}
