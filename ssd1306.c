#include "ssd1306.h"
#include "i2c.h"

#define SSD1306_I2C_ADDR 0x78

void ssd1306_init(void)
{
  i2c_write_reg(0x78, 0x00, 0xae);
  i2c_write_reg(0x78, 0x00, 0x20);
  i2c_write_reg(0x78, 0x00, 0x10);
  i2c_write_reg(0x78, 0x00, 0xb0);
  i2c_write_reg(0x78, 0x00, 0x00);
  i2c_write_reg(0x78, 0x00, 0x10);
  i2c_write_reg(0x78, 0x00, 0x40);
  i2c_write_reg(0x78, 0x00, 0x81);
  i2c_write_reg(0x78, 0x00, 0xff);
  i2c_write_reg(0x78, 0x00, 0xa1);
  i2c_write_reg(0x78, 0x00, 0xa6);
  i2c_write_reg(0x78, 0x00, 0xa8);
  i2c_write_reg(0x78, 0x00, 0x3f);
  i2c_write_reg(0x78, 0x00, 0xa4);
  i2c_write_reg(0x78, 0x00, 0xd3);
  i2c_write_reg(0x78, 0x00, 0x00);
  i2c_write_reg(0x78, 0x00, 0xd3);
  i2c_write_reg(0x78, 0x00, 0xf0);
  i2c_write_reg(0x78, 0x00, 0xd9);
  i2c_write_reg(0x78, 0x00, 0x22);
  i2c_write_reg(0x78, 0x00, 0xda);
  i2c_write_reg(0x78, 0x00, 0x12);
  i2c_write_reg(0x78, 0x00, 0xdb);
  i2c_write_reg(0x78, 0x00, 0x20);
  i2c_write_reg(0x78, 0x00, 0x8d);
  i2c_write_reg(0x78, 0x00, 0x14);
  i2c_write_reg(0x78, 0x00, 0xaf);
}
