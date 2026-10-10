#ifndef AS5600_H
#define AS5600_H
#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

/* Initialise one dedicated I2C bus for an AS5600. */
void as5600_bus_init(i2c_inst_t *i2c, uint sda, uint scl);
/* Raw 12-bit angle (0..4095). Returns false on an I2C error. */
bool as5600_read_raw(i2c_inst_t *i2c, uint16_t *raw);
/* Degrees x100 (0..35999) from the raw count. */
int16_t as5600_raw_to_cdeg(uint16_t raw);
#endif
