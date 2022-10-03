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
  CMD2(0x81, __value)

#define DISPL_ON_RESUME_RAM 0
#define DISPL_ON_IGNORE_RAM 1
#define CMD_DISPL_SET_TURN_ON_BEHAVIOR(__value) \
  CMD(0xa4 | ((__value) & 1))

#define DISPL_INVERTED_OFF 0
#define DISPL_INVERTED_ON  1
#define CMD_SET_INVERTED(__value) \
  CMD(0xa6 | ((__value) & 1))

#define MEMORY_ADDRESSING_HORIZ 0b00
#define MEMORY_ADDRESSING_VERTICAL 0b01
#define MEMORY_ADDRESSING_PAGE 0b10

#define CMD_MEMORY_ADDRESSING(__value) \
  CMD2(0x20, ((__value) & 0x3))

/* Page addressing only */
#define CMD_SET_COL_LO(__value) \
  CMD((__value) & 0xf)

/* Page addressing only */
#define CMD_SET_COL_HI(__value) \
  CMD(0x10 | ((__value) & 0xf))

/* Page addressing only */
#define CMD_SET_PAGE_START_ADDRESS(__start) \
  CMD(0xb0 | ((__start) & 7))

/* Vertical and horizontal addressing */
#define CMD_SET_COLUMN_ADDRESS(__start, __end) \
  CMD3(0x21, (__start) & 0x7f, (__end) & 0x7f)

/* Vertical and horizontal addressing */
#define CMD_SET_PAGE_ADDRESS(__start, __end) \
  CMD3(0x22, (__start) & 0x07, (__end) & 0x07)

/* Hardware configuration */
#define CMD_SET_START_LINE_ADDR(__v) \
  CMD(0x40 | ((__v) & 0x3f))

#define CMD_SET_SEGMENT_REMAP(__v) \
  CMD(0xa0 | ((__v) & 1));

#define CMD_SET_MULTIPLEX_RATIO(__v) \
  CMD2(0xa8, (__v) & 0x3f)

#define COM_SCAN_UP 0
#define COM_SCAN_DOWN 1

#define CMD_SET_COM_SCAN_DIR(__v) \
  CMD(0xc0 | (((__v) & 1) << 3))

#define CMD_SET_DISPL_OFFSET(__v) \
  CMD2(0xd3, ((__v) & 0x3f))

#define CMD_SET_COM_PINS_HW_CONF(__a5, __a4) \
  CMD2(0xda, 0x02 | ((__a5) << 5) | ((__a4) << 4))

/* Timing and driving config */
#define CMD_SET_CLK_DIV_RATIO(__freq, __div) \
  CMD2(0xd5, ((__div) & 0x0f) | (((__freq) << 4) & 0xf0))

#define CMD_SET_PRECHARGE_PERIOD(__v0, __v1) \
  CMD2(0xd9, ((__v0 << 4) & 0xf0) | ((__v1 << 0) & 0x0f))

#define CMD_SET_VCOM_DESELECT_LEVEL(__v1) \
  CMD2(0xdb, 0x30)

#define CMD_CHARGE_PUMP_ENA()\
  CMD2(0x8d, 0x14)

#define SIZE_X 128
#define SIZE_Y 64

static uint8_t display_buf[SIZE_X * SIZE_Y / 8];

void dbuffer_init(void)
{
  for (int i = 0; i < sizeof(display_buf); ++i)
    display_buf[i] = 0x07;
}

void dbuffer_flush(void)
{
  CMD_SET_COL_LO(0);
  CMD_SET_COL_HI(0);
  for (int page = 4; page < 8; ++page) {
    CMD_SET_PAGE_START_ADDRESS(page);
    for (int col = 0; col < 128 / 2; ++col) {
      for (int i = 0; i < 20000; ++i);
      DATA2(display_buf[page * 128 + col], display_buf[page * 128 + col + 1]);
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
  CMD_SET_MULTIPLEX_RATIO(0x3f);
  CMD_SET_DISPL_OFFSET(0);
  CMD_SET_START_LINE_ADDR(0);
  CMD_SET_SEGMENT_REMAP(0);
  CMD_SET_COM_SCAN_DIR(COM_SCAN_DOWN);
  CMD_SET_COM_PINS_HW_CONF(0, 0);
  CMD_SET_CONTRAST(0x7f);
  CMD_DISPL_SET_ON_OFF(DISPL_OFF);
  CMD_SET_INVERTED(DISPL_INVERTED_OFF);
  CMD_SET_CLK_DIV_RATIO(1, 1);
  CMD_MEMORY_ADDRESSING(MEMORY_ADDRESSING_PAGE);
  CMD_SET_COL_HI(0);
  CMD_SET_COL_LO(0);
  CMD_SET_PAGE_START_ADDRESS(0);
  CMD_DISPL_SET_TURN_ON_BEHAVIOR(DISPL_ON_RESUME_RAM);
  CMD_SET_PRECHARGE_PERIOD(0x2, 0x2);
  CMD_SET_VCOM_DESELECT_LEVEL(2);
  CMD_CHARGE_PUMP_ENA();
  CMD_DISPL_SET_ON_OFF(DISPL_ON);
  dbuffer_init();
  dbuffer_flush();
  while(1);

  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0x00);
#if 0
  for (int i = 0; i < sizeof(display_buf); ++i) {
    i2c_read_bytes1(SSD1306_I2C_ADDR, 0x40, &display_buf[i]);
  }
#endif
  
  CMD_SET_COL_LO(0);
  CMD_SET_COL_HI(0);
  for (int i = 0; i < 128 * 64 / 8; ++i)  {
    for (volatile int z = 0; z < 200000; ++z);
    i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, i & 0xff);
  }
  i2c_write_bytes1(SSD1306_I2C_ADDR, 0x40, 0x00);
  b ();
}
