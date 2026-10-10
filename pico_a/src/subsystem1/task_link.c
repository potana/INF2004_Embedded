/* Subsystem 1: UART Link Task (Pico A). Priority 4.
 * Owns UART0. Decodes frames from Pico B, sends SETTLED / FAULT / HEARTBEAT. */
#include "app.h"
#include "pins_a.h"
#include "subsystem1/protocol.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"

#define HEARTBEAT_PERIOD_MS 100

static void send(uint8_t type, uint8_t seq, const uint8_t *payload, uint8_t len)
{
    frame_t f = { .type = type, .seq = seq, .len = len };
    for (uint8_t i = 0; i < len; i++) f.payload[i] = payload[i];
    uint8_t buf[FRAME_MAX_LEN];
    size_t n = frame_encode(&f, buf);
    if (n) uart_write_blocking(LINK_UART, buf, n);
}

void task_link(void *arg)
{
    (void)arg;
    uart_init(LINK_UART, LINK_BAUD);
    gpio_set_function(PIN_LINK_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_LINK_RX, GPIO_FUNC_UART);

    frame_parser_t parser;
    frame_parser_init(&parser);
    TickType_t last_hb = xTaskGetTickCount();
    uint8_t settled_seq_sent = 0xFF;
    uint8_t hb_seq = 0;

    for (;;) {
        /* Drain RX. TODO(S1 owner): replace polling with a UART interrupt + stream buffer. */
        while (uart_is_readable(LINK_UART)) {
            frame_t f;
            if (!frame_parser_push(&parser, uart_getc(LINK_UART), &f)) continue;
            switch (f.type) {
            case MSG_MOVE:
                if (f.len == 4) {
                    move_cmd_t cmd = { .seq = f.seq,
                                       .target = { get_i16(&f.payload[0]), get_i16(&f.payload[2]) } };
                    xQueueSend(q_move, &cmd, 0);
                }
                break;
            case MSG_STOP:
                xEventGroupSetBits(eg_faults, FAULT_STOP_CMD);
                send(MSG_ACK, f.seq, (uint8_t[]){ MSG_STOP }, 1);
                break;
            case MSG_HEARTBEAT:
                g_last_heartbeat_ms = to_ms_since_boot(get_absolute_time());
                break;
            default: break;
            }
        }

        /* Report SETTLED once per accepted MOVE. TODO(S2 owner): carry the real seq via the motor task. */
        if (g_settled && settled_seq_sent != 0) {
            uint8_t p[5];
            put_i16(&p[0], g_pose.tilt_cdeg);
            put_i16(&p[2], g_pose.sweep_cdeg);
            p[4] = 1;
            send(MSG_SETTLED, 0, p, 5);
            settled_seq_sent = 0;
        } else if (!g_settled) {
            settled_seq_sent = 0xFF;
        }

        if (xTaskGetTickCount() - last_hb >= pdMS_TO_TICKS(HEARTBEAT_PERIOD_MS)) {
            send(MSG_HEARTBEAT, hb_seq++, (uint8_t[]){ safety_motors_enabled() ? 1 : 0 }, 1);
            last_hb += pdMS_TO_TICKS(HEARTBEAT_PERIOD_MS);
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}
