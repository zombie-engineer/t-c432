#pragma once
#include <stdint.h>

void i2c_clock_setup(void);
void i2c_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
