#include "ssd1306.h"
#include "i2c.h"

#define SSD1306_I2C_ADDR 0x78

#define DATA(__byte) \
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, __byte)

#define DATA2(__byte0, __byte1) \
  i2c_write_bytes2(SSD1306_I2C_ADDR, 0x40, __byte0, __byte1)

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

#define CMD_SET_COL_LO(__value) \
  CMD((__value) & 0xf)

#define CMD_SET_CO_HI(__value) \
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

#define CMD_SET_PAGE_START_ADDRESS(__start) \
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

#define SIZE_X 128
#define SIZE_Y 64
static uint8_t display_buf[SIZE_X * SIZE_Y / 8];

void dbuffer_init(void)
{
  for (int i = 0; i < sizeof(display_buf); ++i)
    display_buf[i] = 0xff;
}

void dbuffer_flush(void)
{
  for (int page = 0; page < 4; ++page) {
    CMD_SET_PAGE_START_ADDRESS(page);
    for (int col = 0; col < 128; ++col) {
      DATA(display_buf[page * 128 + col]);
    }
  }
}

int dbuffer_get_pixel(int x, int y)
{
  int page_idx = y / SIZE_Y;
  int bitpos = y % SIZE_Y;
  int byte_idx = page_idx * SIZE_X + x;
  uint8_t b = display_buf[byte_idx];
  return (b >> bitpos) & 1;
}

void dbuffer_draw_pixel(int x, int y, int color)
{
  int page_idx = y / SIZE_Y;
  int bitpos = y % SIZE_Y;
  int byte_idx = page_idx * SIZE_X + x;
  uint8_t b = display_buf[byte_idx];
  b &= ~(uint8_t)(1<<bitpos);
  b |= (uint8_t)((color & 1)<<bitpos);
  display_buf[byte_idx] = b;
}

extern void b(void);

void ssd1306_init(void)
{
  CMD_DISPL_SET_ON_OFF(DISPL_OFF);
  CMD_MEMORY_ADDRESSING(MEMORY_ADDRESSING_PAGE);
  CMD_SET_CO_HI(0);
  CMD_SET_COL_LO(0);
  CMD_SET_PAGE_START_ADDRESS(0);
  CMD_SET_COM_SCAN_DIR(COM_SCAN_DOWN);
  CMD_SET_CO_HI(0);
  CMD_SET_START_LINE_ADDR(0);
  CMD_SET_CONTRAST(0xff);
  CMD_SET_SEGMENT_REMAP(0);
  CMD_SET_INVERTED(DISPL_INVERTED_OFF);
  CMD_SET_MULTIPLEX_RATIO(0x3f);
  CMD_DISPL_SET_TURN_ON_BEHAVIOR(DISPL_ON_RESUME_RAM);
  CMD_SET_DISPL_OFFSET(0);
  CMD_SET_CLK_DIV_RATIO(0x00);
  CMD_SET_PRECHARGE_PERIOD(1, 0);
  CMD_SET_COM_PINS_HW_CONF(1, 0);
  CMD_SET_VCOM_DESELECT_LEVEL(2);
  CMD_CHARGE_PUMP_ENA();
  CMD_DISPL_SET_ON_OFF(DISPL_ON);

  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0x00);
  CMD_SET_COL_LO(0);
  CMD_SET_CO_HI(0);
  for (int i = 0; i < sizeof(display_buf); ++i) {
    i2c_read_bytes1(SSD1306_I2C_ADDR, 0x40, &display_buf[i]);
  }
  b ();
  CMD_SET_COL_LO(0);
  CMD_SET_CO_HI(0);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0x00);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0x00);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0x00);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0x00);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0xff);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0xff);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0xff);
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0);
  b ();
  
}
