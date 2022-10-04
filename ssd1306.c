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
  CMD2(0x81,(__value))

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

#define CMD_SET_COL(__x) \
  CMD_SET_COL_LO((__x) & 0xf); \
  CMD_SET_COL_HI(((__x) >> 4) & 0xf)


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
  CMD2(0xda, 0x12);
//  CMD2(0xda, 0x02 | ((__a5) << 5) | ((__a4) << 4))

/* Timing and driving config */
#define CMD_SET_CLK_DIV_RATIO(__freq, __div) \
  CMD2(0xd5, ((__div) & 0x0f) | (((__freq) << 4) & 0xf0))

#define CMD_SET_PRECHARGE_PERIOD(__v0, __v1) \
  CMD2(0xd9, ((__v0 << 4) & 0xf0) | ((__v1 << 0) & 0x0f))

#define CMD_SET_VCOM_DESELECT_LEVEL(__v1) \
  CMD2(0xdb, (__v1) << 4)

#define CMD_CHARGE_PUMP_ENA()\
  CMD2(0x8d, 0x14)

#define SIZE_X 128
#define SIZE_Y 64
#define ROWS_PER_PAGE 8
#define NUM_PAGES (SIZE_Y / ROWS_PER_PAGE)
#define NUM_COLUMNS SIZE_X

static uint8_t dbuf[SIZE_X * SIZE_Y / 8];


#define DBYTE(__col, __page) (dbuf[__col + __page * SIZE_X])

void dbuf_clear(void)
{
  int page;
  int col;

  for (page = 0; page < NUM_PAGES; ++page) {
    for (col = 0; col < NUM_COLUMNS; ++col) {
      DBYTE(col, page)  = 0;
    }
  }
}

void dbuf_init(void)
{
  dbuf_clear();
}

void dbuf_draw_pixel(int x, int y, int color)
{
  uint8_t *p = &DBYTE(x, y / ROWS_PER_PAGE);
  int bitidx = y % ROWS_PER_PAGE;
  *p = (*p & ~(1<<bitidx)) | (1<<bitidx);
}

void dbuf_draw_line(int x0, int y0, int x1, int y1, int color)
{
  if (x1 - x0 > y1 - y0)  {
    float slope = (float)(y1 - y0) / (x1 - x0);
    for (int x = x0; x <= x1; x++) {
      int y = y0 + slope * (x - x0);
      dbuf_draw_pixel(x, y, color);
    }
  } else {
    float slope = (float)(x1 - x0) / (y1 - y0);
    for (int y = y0; y <= y1; y++) {
      int x = x0 + slope * (y - y0);
      dbuf_draw_pixel(x, y, color);
    }
  }
}

int dbuf_get_pixel(int x, int y)
{
  int page_idx = y / SIZE_Y;
  int bitpos = y % SIZE_Y;
  int byte_idx = page_idx * SIZE_X + x;
  uint8_t b = dbuf[byte_idx];
  return (b >> bitpos) & 1;
}

void dbuf_flush(void)
{
  int page;
  int col;

  CMD_SET_COL(0);
  for (page = 0; page < NUM_PAGES; ++page) {
    CMD_SET_PAGE_START_ADDRESS(page);
    i2c_write_bytes_x(SSD1306_I2C_ADDR, 0x40, &DBYTE(0, page), NUM_COLUMNS);
  }
}

extern void b(void);

void ssd1306_horizontal_scroll(int start_page, int end_page, int duration)
{
  char buf[8] = {
    0x00,
    0x26,
    0x00,
    start_page & 7,
    duration & 7,
    end_page & 7,
    0x00,
    0xff
  };

  i2c_write_bytes(SSD1306_I2C_ADDR, buf, 8);
  CMD(0x2f);
}

void ssd1306_vertical_scroll(int start_page, int end_page, int duration)
{
  char buf[7] = {
    0x00,
    0x2a,
    0x00,
    start_page & 7,
    duration & 7,
    end_page & 7,
    0 
  };

  i2c_write_bytes(SSD1306_I2C_ADDR, buf, 7);
  CMD(0x2f);
}

void ssd1306_arg(uint16_t arg)
{
  CMD_SET_CONTRAST(arg >> 4);
}

void ssd1306_init(void)
{
  CMD_SET_MULTIPLEX_RATIO(0x3f);
  CMD_SET_DISPL_OFFSET(0);
  CMD_SET_START_LINE_ADDR(0);
  CMD_SET_SEGMENT_REMAP(0);
  CMD_SET_COM_SCAN_DIR(COM_SCAN_DOWN);
  CMD_SET_COM_PINS_HW_CONF(1, 0);
  CMD_SET_CONTRAST(0x7f);
  CMD_DISPL_SET_ON_OFF(DISPL_OFF);
  CMD_SET_INVERTED(DISPL_INVERTED_OFF);
  CMD_SET_CLK_DIV_RATIO(15, 0);
  CMD_MEMORY_ADDRESSING(MEMORY_ADDRESSING_PAGE);
  CMD_SET_COL(0);
  CMD_SET_PAGE_START_ADDRESS(0);
  CMD_DISPL_SET_TURN_ON_BEHAVIOR(DISPL_ON_RESUME_RAM);
  CMD_SET_PRECHARGE_PERIOD(0x1, 0x1);
  CMD_SET_VCOM_DESELECT_LEVEL(1);
  CMD_CHARGE_PUMP_ENA();
  CMD_DISPL_SET_ON_OFF(DISPL_ON);
  dbuf_init();
  dbuf_flush();
}
