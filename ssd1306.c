#include "ssd1306.h"
#include "i2c.h"

#define SSD1306_I2C_ADDR 0x78

#define CMD(__cmdbyte) \
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x00, (__cmdbyte))

#define CMD2(__cmdbyte, __valuebyte) \
  i2c_write_bytes2(SSD1306_I2C_ADDR, 0x00, (__cmdbyte), (__valuebyte))

#define CMD3(__cmdbyte, __valuebyte0, __valuebyte1) \
  i2c_write_bytes3(SSD1306_I2C_ADDR, 0x00, (__cmdbyte), (__valuebyte0), (__valuebyte1))

#define DISPL_OFF 0
#define DISPL_ON 1
#define CMD_DISPL_SET_ON_OFF(__value) \
  CMD(0xae | ((__value) & 1))

#define CMD_DISPL_AT_TURN_ON2() \
  CMD(0xaf)

#define CMD_SET_CONTRAST(__value) \
  CMD2(0x81, (__value))

#define DISPL_ON_RESUME_RAM 0
#define DISPL_ON_IGNORE_RAM 1
#define CMD_DISPL_SET_TURN_ON_BEHAVIOR(__value) \
  CMD(0xa4 | ((__value) & 1))

#define DISPL_INVERTED_OFF 0
#define DISPL_INVERTED_ON  1
#define CMD_SET_INVERTED(__value) \
  CMD(0xa6 | ((__value) & 1))

#define CMD_SET_LOWER_COL(__value) \
  CMD((__value) & 0xf)

#define CMD_SET_HIGH_COL(__value) \
  CMD(0x10 | ((__value) & 0xf))

#define MEMORY_ADDRESSING_HORIZ 0b00
#define MEMORY_ADDRESSING_VERTICAL 0b01
#define MEMORY_ADDRESSING_PAGE 0b10

#define CMD_MEMORY_ADDRESSING(__value) \
  CMD2(0x20, ((__value) & 0x3))

#define CMD_SET_COLUMN_ADDRESS(__start, __end) \
  CMD3(0x21, (__start) & 0x3f, (__end) & 0x3f)

#define CMD_SET_PAGE_ADDRESS(__start, __end) \
  CMD3(0x22, (__start) & 0x07, (__end) & 0x07)

#define CMD_SET_PAGE_START(__start) \
  CMD(0xb0 | ((__start) & 7))

#define COM_SCAN_UP 0
#define COM_SCAN_DOWN 1

#define CMD_SET_COM_SCAN_DIR(__v) \
  CMD(0xc8 | ((__v) & 1))

#define CMD_SET_START_LINE_ADDR(__v) \
  CMD(0x40 | ((__v) & 0x3f))

#define CMD_SET_DISPL_OFFSET(__v) \
  CMD2(0xd3, ((__v) & 0x3f))

#define CMD_SET_CLK_DIV_RATIO(__v) \
  CMD2(0xd5, (__v) & 0xff)

#define CMD_SET_PRECHARGE_PERIOD(__v1, __v2) \
  CMD(0xd9 | ((__v1) & 0x0f) | (((__v2) << 4) & 0xf0))

#define CMD_SET_COM_PINS_HW_CONF(__v1, __v2) \
  CMD2(0xda, (((__v1) & 1)<< 4) | (((__v2) & 1) << 5))

#define CMD_SET_VCOM_DESELECT_LEVEL(__v1) \
  CMD2(0xda, (((__v1) & 7)<< 4))

#define CMD_CHARGE_PUMP_ENA()\
  CMD2(0x8d, 0x14)

#define CMD_SET_SEGMENT_REMAP(__v) \
  CMD(0xa0 | ((__v) & 1));

#define CMD_SET_MULTIPLEX_RATIO(__v) \
  CMD2(0xa8, 0xa0 | (__v) & 0x3f)
void ssd1306_init(void)
{
  CMD_DISPL_SET_ON_OFF(DISPL_OFF);
#if 0
  CMD_MEMORY_ADDRESSING(MEMORY_ADDRESSING_HORIZ);
  CMD_SET_HIGH_COL(0);
  CMD_SET_PAGE_START(0);
  CMD_SET_COM_SCAN_DIR(COM_SCAN_DOWN);
  CMD_SET_LOWER_COL(0);
  CMD_SET_HIGH_COL(0);
  CMD_SET_START_LINE_ADDR(0);
  CMD_SET_CONTRAST(0xff);
  CMD_SET_SEGMENT_REMAP(0);
  CMD_SET_INVERTED(DISPL_INVERTED_OFF);
  CMD_SET_MULTIPLEX_RATIO(0x3f);
  CMD_DISPL_SET_TURN_ON_BEHAVIOR(DISPL_ON_RESUME_RAM);
  CMD_SET_DISPL_OFFSET(0);
  CMD_SET_CLK_DIV_RATIO(0x00);
  CMD_SET_PRECHARGE_PERIOD(1, 0);
#endif
  CMD_SET_COM_PINS_HW_CONF(1, 0);
  CMD_SET_VCOM_DESELECT_LEVEL(2);
  CMD_CHARGE_PUMP_ENA();
  for (volatile int i = 0; i < 200000; ++i);
  CMD_DISPL_SET_ON_OFF(DISPL_ON);
  while(1) {
  for (volatile int x = 0; x < 128; ++x) {
    i2c_write_bytes1(0x78, 0x40, 0x1);
  }
  }


  int r = 0;
  while(1) {
  for (int i = 0; i < 0xffffffff; ++i) {
    r++;
    CMD_SET_COLUMN_ADDRESS(59+ r , 62 + r);
    i2c_write_bytes1(0x78, 0x40, 0xff);
    i2c_write_bytes1(0x78, 0x40, 0x00);
    i2c_write_bytes1(0x78, 0x40, 0x00);
    i2c_write_bytes1(0x78, 0x40, 0x00);
    CMD_SET_COLUMN_ADDRESS(0, 3);
    i2c_write_bytes1(0x78, 0x40, 0xff);
    i2c_write_bytes1(0x78, 0x40, 0xff);
    i2c_write_bytes1(0x78, 0x40, 0xff);

    CMD_SET_COLUMN_ADDRESS(0, 3);
    i2c_write_bytes1(0x78, 0x40, 0xff);
    i2c_write_bytes1(0x78, 0x40, 0xff);
    i2c_write_bytes1(0x78, 0x40, 0xff);
  }
  }
  while(1);
}
