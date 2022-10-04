#pragma once
#include <stdint.h>

void i2c_clock_setup(void);
void i2c_write_bytes(uint8_t i2c_addr, uint8_t *data, int count);
void i2c_write_bytes_x(uint8_t i2c_addr, uint8_t cmdbyte, uint8_t *data, int count);
void i2c_write_bytes1(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
void i2c_read_bytes1(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data);
void i2c_write_bytes2(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1);
void i2c_write_bytes3(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1, uint8_t data2);
