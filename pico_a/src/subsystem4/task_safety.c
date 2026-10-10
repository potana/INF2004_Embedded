/* Subsystem 4: Safety Monitor Task (Pico A). Priority 6 (highest).
 * Owns the EN1/EN2 pins. Any fault, a lost heartbeat, or a STOP command
 * drops both enables. Motors stay off until faults are cleared and the
 * link is alive again. */
#include "app.h"
#include "pins_a.h"
#include "pico/stdlib.h"
#include "task.h"

#define HEARTBEAT_TIMEOUT_MS 500   /* design review, Section 4 */
#define SAFETY_PERIOD_MS     10

static volatile bool s_enabled = false;
static volatile bool s_enable_requested = false;

void safety_disable_motors(void)
{
    gpio_put(PIN_M1_EN, 0);
    gpio_put(PIN_M2_EN, 0);
    s_enabled = false;
}
bool safety_motors_enabled(void) { return s_enabled; }
void safety_request_enable(void) { s_enable_requested = true; }

void task_safety(void *arg)
{
    (void)arg;
    gpio_init(PIN_M1_EN); gpio_set_dir(PIN_M1_EN, GPIO_OUT);
    gpio_init(PIN_M2_EN); gpio_set_dir(PIN_M2_EN, GPIO_OUT);
    safety_disable_motors();                       /* low during boot */

    TickType_t last = xTaskGetTickCount();
    for (;;) {
        EventBits_t faults = xEventGroupGetBits(eg_faults);
        uint32_t now_ms = to_ms_since_boot(get_absolute_time());
        bool link_alive = g_last_heartbeat_ms != 0 && (now_ms - g_last_heartbeat_ms) < HEARTBEAT_TIMEOUT_MS;

        if (g_last_heartbeat_ms != 0 && !link_alive) { xEventGroupSetBits(eg_faults, FAULT_LINK_TIMEOUT); faults |= FAULT_LINK_TIMEOUT; }

        if (faults) {
            safety_disable_motors();
            s_enable_requested = false;
            /* TODO(S4 owner): send MSG_FAULT to Pico B via the link task, and define the reset sequence. */
        } else if (link_alive && s_enable_requested) {
            gpio_put(PIN_M1_EN, 1);
            gpio_put(PIN_M2_EN, 1);
            s_enabled = true;
        }
        vTaskDelayUntil(&last, pdMS_TO_TICKS(SAFETY_PERIOD_MS));
    }
}
