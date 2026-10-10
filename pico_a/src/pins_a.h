#ifndef PINS_A_H
#define PINS_A_H
/* Pico WH A pin map. Source of truth: design review, Section 2.1.
 * Keep this file and TEAM.md (pin table) identical. */

/* Motor 1 (tilt): three PWM phase inputs. GP12 and GP13 share PWM slice 6,
 * so both motors must use the same PWM frequency. */
#define PIN_M1_IN1   10
#define PIN_M1_IN2   11
#define PIN_M1_IN3   12
/* Motor 2 (sweep) */
#define PIN_M2_IN4   13
#define PIN_M2_IN5   14
#define PIN_M2_IN6   15
/* Driver enables: low during boot and on fault. */
#define PIN_M1_EN    16
#define PIN_M2_EN    17

/* AS5600 encoders, one dedicated I2C bus each, both at address 0x36. */
#define PIN_ENC1_SDA 4
#define PIN_ENC1_SCL 5
#define PIN_ENC2_SDA 6
#define PIN_ENC2_SCL 7
#define ENC1_I2C     i2c0
#define ENC2_I2C     i2c1
#define AS5600_ADDR  0x36

/* UART0 to Pico B: A.TX(GP0) -> B.RX(GP17), A.RX(GP1) <- B.TX(GP16), common GND. */
#define PIN_LINK_TX  0
#define PIN_LINK_RX  1
#define LINK_UART    uart0
#define LINK_BAUD    115200

/* Driver RESET / SLEEP / FAULT: not assigned yet (suggested GP18 for FAULT). */
#endif
