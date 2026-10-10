/* Subsystem 2: Motor Control Task (Pico A). Priority 5.
 * Owns the six motor PWM outputs and both AS5600 I2C buses.
 * TODO(S2 owner): closed-loop FOC. This skeleton reads the encoders, keeps the
 * outputs idle, and reports SETTLED when the measured pose is within tolerance. */
#include "app.h"
#include "pins_a.h"
#include "subsystem2/as5600.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "task.h"

#define SETTLE_TOL_CDEG  100   /* 1.0 degree */
#define SETTLE_DWELL_MS  50
#define PWM_WRAP         999   /* 125 MHz / 1000 = 125 kHz; GP12 and GP13 share a slice, one frequency for both */

static const uint motor_pins[6] = { PIN_M1_IN1, PIN_M1_IN2, PIN_M1_IN3, PIN_M2_IN4, PIN_M2_IN5, PIN_M2_IN6 };

static void pwm_outputs_init(void)
{
    for (int i = 0; i < 6; i++) {
        gpio_set_function(motor_pins[i], GPIO_FUNC_PWM);
        uint slice = pwm_gpio_to_slice_num(motor_pins[i]);
        pwm_set_wrap(slice, PWM_WRAP);
        pwm_set_gpio_level(motor_pins[i], 0);
        pwm_set_enabled(slice, true);
    }
}

void task_motor(void *arg)
{
    (void)arg;
    as5600_bus_init(ENC1_I2C, PIN_ENC1_SDA, PIN_ENC1_SCL);
    as5600_bus_init(ENC2_I2C, PIN_ENC2_SDA, PIN_ENC2_SCL);
    pwm_outputs_init();

    setpoint_t sp = { 0 };
    bool have_sp = false;
    TickType_t in_tol_since = 0;
    TickType_t last = xTaskGetTickCount();

    for (;;) {
        setpoint_t next;
        if (xQueueReceive(q_setpoint, &next, 0) == pdTRUE) { sp = next; have_sp = true; g_settled = false; in_tol_since = 0; }

        uint16_t r1, r2;
        bool ok1 = as5600_read_raw(ENC1_I2C, &r1);
        bool ok2 = as5600_read_raw(ENC2_I2C, &r2);
        if (!ok1) xEventGroupSetBits(eg_faults, FAULT_ENCODER_1);
        if (!ok2) xEventGroupSetBits(eg_faults, FAULT_ENCODER_2);
        if (ok1 && ok2) {
            pose_t p = { as5600_raw_to_cdeg(r1), as5600_raw_to_cdeg(r2) };
            g_pose = p;
            if (have_sp && safety_motors_enabled()) {
                int dt = p.tilt_cdeg - sp.target.tilt_cdeg, ds = p.sweep_cdeg - sp.target.sweep_cdeg;
                bool inside = (dt > -SETTLE_TOL_CDEG && dt < SETTLE_TOL_CDEG && ds > -SETTLE_TOL_CDEG && ds < SETTLE_TOL_CDEG);
                if (!inside) in_tol_since = 0;
                else if (in_tol_since == 0) in_tol_since = xTaskGetTickCount();
                else if (xTaskGetTickCount() - in_tol_since >= pdMS_TO_TICKS(SETTLE_DWELL_MS)) g_settled = true;
                /* TODO(S2 owner): compute the three phase duties for each motor and write them here. */
            }
        }
        vTaskDelayUntil(&last, pdMS_TO_TICKS(5));  /* loop rate TBD (design review, Section 3) */
    }
}
