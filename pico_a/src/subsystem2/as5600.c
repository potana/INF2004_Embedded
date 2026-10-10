#include "as5600.h"
#include "pins_a.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define REG_RAW_ANGLE_H 0x0C

void as5600_bus_init(i2c_inst_t *i2c, uint sda, uint scl)
{
    i2c_init(i2c, 400 * 1000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);   /* TODO(S2 owner): drop if the breakout has pull-ups (design review, 2.3) */
    gpio_pull_up(scl);
}

bool as5600_read_raw(i2c_inst_t *i2c, uint16_t *raw)
{
    uint8_t reg = REG_RAW_ANGLE_H, d[2];
    if (i2c_write_timeout_us(i2c, AS5600_ADDR, &reg, 1, true, 2000) != 1) return false;
    if (i2c_read_timeout_us(i2c, AS5600_ADDR, d, 2, false, 2000) != 2) return false;
    *raw = (uint16_t)(((d[0] & 0x0F) << 8) | d[1]);
    return true;
}

int16_t as5600_raw_to_cdeg(uint16_t raw)
{
    return (int16_t)(((uint32_t)raw * 36000u) / 4096u);
}
