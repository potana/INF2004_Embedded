#include "protocol.h"
#include <string.h>

uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int b = 0; b < 8; b++)
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
    }
    return crc;
}

void put_i16(uint8_t *dst, int16_t v) { dst[0] = (uint8_t)((uint16_t)v >> 8); dst[1] = (uint8_t)v; }
int16_t get_i16(const uint8_t *src)   { return (int16_t)(((uint16_t)src[0] << 8) | src[1]); }

size_t frame_encode(const frame_t *f, uint8_t *out)
{
    if (f->len > FRAME_MAX_PAYLOAD) return 0;
    out[0] = FRAME_SOF;
    out[1] = f->type;
    out[2] = f->seq;
    out[3] = f->len;
    memcpy(&out[4], f->payload, f->len);
    uint16_t crc = crc16_ccitt(&out[1], 3u + f->len);
    out[4 + f->len] = (uint8_t)(crc >> 8);
    out[5 + f->len] = (uint8_t)crc;
    return 6u + f->len;
}

void frame_parser_init(frame_parser_t *p) { p->n = 0; }

bool frame_parser_push(frame_parser_t *p, uint8_t byte, frame_t *f)
{
    if (p->n == 0) {                      /* hunting for SOF */
        if (byte == FRAME_SOF) p->buf[p->n++] = byte;
        return false;
    }
    p->buf[p->n++] = byte;
    if (p->n == 4 && p->buf[3] > FRAME_MAX_PAYLOAD) { p->n = 0; return false; }
    if (p->n >= 4 && p->n == 6u + p->buf[3]) {
        uint8_t len = p->buf[3];
        uint16_t want = (uint16_t)((p->buf[4 + len] << 8) | p->buf[5 + len]);
        p->n = 0;
        if (crc16_ccitt(&p->buf[1], 3u + len) != want) return false;
        f->type = p->buf[1];
        f->seq = p->buf[2];
        f->len = len;
        memcpy(f->payload, &p->buf[4], len);
        return true;
    }
    return false;
}
