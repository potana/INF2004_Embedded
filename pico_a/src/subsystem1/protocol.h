#ifndef PROTOCOL_H
#define PROTOCOL_H
/* Pico-to-Pico UART frame. PROPOSED: the subsystem 1 owner finalises it.
 * Mirror: pico_b/subsystem1/protocol.py. Keep both identical.
 *
 *   [0xA5][type][seq][len][payload x len][crc_hi][crc_lo]
 *
 * CRC is CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF) over type..payload.
 * Angles are int16 centi-degrees, big-endian. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FRAME_SOF        0xA5
#define FRAME_MAX_PAYLOAD 8
#define FRAME_MAX_LEN    (4 + FRAME_MAX_PAYLOAD + 2)

enum {
    MSG_MOVE      = 1,  /* B -> A  payload: tilt i16, sweep i16        */
    MSG_SETTLED   = 2,  /* A -> B  payload: tilt i16, sweep i16, flag  */
    MSG_STOP      = 3,  /* B -> A  payload: reason u8                  */
    MSG_FAULT     = 4,  /* A -> B  payload: fault id u8, tilt i16, sweep i16 */
    MSG_HEARTBEAT = 5,  /* both    payload: state u8                   */
    MSG_ACK       = 6,  /* both    payload: acked type u8 (answers STOP) */
};

typedef struct {
    uint8_t type;
    uint8_t seq;
    uint8_t len;
    uint8_t payload[FRAME_MAX_PAYLOAD];
} frame_t;

uint16_t crc16_ccitt(const uint8_t *data, size_t len);

/* Returns bytes written to out (<= FRAME_MAX_LEN), or 0 if the payload is too long. */
size_t frame_encode(const frame_t *f, uint8_t *out);

/* Byte-at-a-time parser. Returns true when a complete, CRC-valid frame is in *f.
 * Resynchronises on the next SOF after any error. */
typedef struct { uint8_t buf[FRAME_MAX_LEN]; uint8_t n; } frame_parser_t;
void frame_parser_init(frame_parser_t *p);
bool frame_parser_push(frame_parser_t *p, uint8_t byte, frame_t *f);

/* Payload helpers. */
void put_i16(uint8_t *dst, int16_t v);
int16_t get_i16(const uint8_t *src);

#endif
