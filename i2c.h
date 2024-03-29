#pragma once
#include <stdint.h>
#include <stdbool.h>

void i2c_init(void);
void i2c_init_isr(bool use_dma);
void i2c_isr_disable(void);
void i2c_clock_setup(void);
void i2c_write_async(uint8_t i2c_addr, const uint8_t *data, int count, bool dma);
void i2c_write_sync(uint8_t i2c_addr, const uint8_t *data, int count);
#if 0
void i2c_write_bytes_x(uint8_t i2c_addr, uint8_t cmdbyte, uint8_t *data, int count);
void i2c_write_async(uint8_t i2c_addr, const uint8_t *data, int count);
void i2c_write_bytes1(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
void i2c_write_bytes1_async(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
void i2c_read_bytes1(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data);
void i2c_write_bytes2(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1);
void i2c_write_bytes2_async(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1);
void i2c_write_bytes3(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1, uint8_t data2);
void i2c_write_bytes3_async(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data0, uint8_t data1, uint8_t data2);
#endif
