import machine

VL51L1X_DEFAULT_CONFIGURATION = bytes([
    0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x02, 0x08, 0x00, 0x08,
    0x10, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x0F,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x0b, 0x00, 0x00, 0x02,
    0x0a, 0x21, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0xc8,
    0x00, 0x00, 0x38, 0xff, 0x01, 0x00, 0x08, 0x00, 0x00, 0x01,
    0xdb, 0x0f, 0x01, 0xf1, 0x0d, 0x01, 0x68, 0x00, 0x80, 0x08,
    0xb8, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x89, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, 0x0d, 0x0e, 0x0e, 0x00,
    0x00, 0x02, 0xc7, 0xff, 0x9B, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x40
])
# Bytes correspond to registers 0x2d-0x87. See the VL53L1X datasheet or
# https://github.com/drakxtwo/vl53l1x_pico for the per-register comments.


class VL53L1X:
    def __init__(self, i2c, address=0x29):
        self.i2c = i2c
        self.address = address
        self.reset()
        machine.lightsleep(1)
        if self.read_model_id() != 0xEACC:
            raise RuntimeError('Failed to find expected ID register values. Check wiring!')
        self.i2c.writeto_mem(self.address, 0x2D, VL51L1X_DEFAULT_CONFIGURATION, addrsize=16)
        # The real API triggers this change in VL53L1_init_and_start_range() once
        # a measurement is started; assumes MM1 and MM2 are disabled.
        self.writeReg16Bit(0x001E, self.readReg16Bit(0x0022) * 4)
        machine.lightsleep(200)

    def writeReg(self, reg, value):
        return self.i2c.writeto_mem(self.address, reg, bytes([value]), addrsize=16)

    def writeReg16Bit(self, reg, value):
        return self.i2c.writeto_mem(self.address, reg, bytes([(value >> 8) & 0xFF, value & 0xFF]), addrsize=16)

    def readReg(self, reg):
        return self.i2c.readfrom_mem(self.address, reg, 1, addrsize=16)[0]

    def readReg16Bit(self, reg):
        data = self.i2c.readfrom_mem(self.address, reg, 2, addrsize=16)
        return (data[0] << 8) + data[1]

    def read_model_id(self):
        return self.readReg16Bit(0x010F)

    def reset(self):
        self.writeReg(0x0000, 0x00)
        machine.lightsleep(100)
        self.writeReg(0x0000, 0x01)

    def read(self):
        # RESULT__RANGE_STATUS block
        data = self.i2c.readfrom_mem(self.address, 0x0089, 17, addrsize=16)
        final_crosstalk_corrected_range_mm_sd0 = (data[13] << 8) + data[14]
        return final_crosstalk_corrected_range_mm_sd0
