/* Subsystem 2: Motion Planner Task (Pico A). Priority 3.
 * Clamps MOVE targets to the mechanical limits and hands setpoints to the motor task. */
#include "app.h"
#include "task.h"

/* TODO(S2/S4 owners): replace with the measured mechanical limits (design review, Section 2.4). */
#define TILT_MIN_CDEG    (-4500)
#define TILT_MAX_CDEG    ( 4500)
#define SWEEP_MIN_CDEG   (-1500)
#define SWEEP_MAX_CDEG   ( 1500)

static int16_t clamp16(int16_t v, int16_t lo, int16_t hi) { return v < lo ? lo : (v > hi ? hi : v); }

void task_planner(void *arg)
{
    (void)arg;
    for (;;) {
        move_cmd_t cmd;
        if (xQueueReceive(q_move, &cmd, portMAX_DELAY) != pdTRUE) continue;
        setpoint_t sp = { .seq = cmd.seq,
                          .target = { clamp16(cmd.target.tilt_cdeg, TILT_MIN_CDEG, TILT_MAX_CDEG),
                                      clamp16(cmd.target.sweep_cdeg, SWEEP_MIN_CDEG, SWEEP_MAX_CDEG) } };
        if (sp.target.tilt_cdeg != cmd.target.tilt_cdeg || sp.target.sweep_cdeg != cmd.target.sweep_cdeg)
            xEventGroupSetBits(eg_faults, FAULT_LIMIT); /* TODO: report "clipped" instead of faulting */
        xQueueOverwrite(q_setpoint, &sp);
    }
}
