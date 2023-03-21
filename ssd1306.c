#include "ssd1306.h"
#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include "config.h"
#include "time.h"
#include "display.h"
#include "compiler.h"

#define SSD1306_I2C_ADDR 0x78
#define SSD1306_ADDRESSING_MODE_HORIZONTAL

#ifdef CONFIG_I2C_ASYNC
#define i2c_write_op i2c_write_async
#else
#define i2c_write_op i2c_write_sync
#endif

static inline int i2c_write2(uint8_t b0, uint8_t b1)
{
  const uint8_t buf[] = {
    b0, b1
  };

  i2c_write_op(SSD1306_I2C_ADDR, buf, sizeof(buf), false);
}

static inline int i2c_write3(uint8_t b0, uint8_t b1, uint8_t b2)
{
  const uint8_t buf[] = {
    b0, b1, b2
  };

  i2c_write_op(SSD1306_I2C_ADDR, buf, sizeof(buf), false);
}

static inline int i2c_write4(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
  const uint8_t buf[] = {
    b0, b1, b2, b3
  };

  i2c_write_op(SSD1306_I2C_ADDR, buf, sizeof(buf), false);
}

#define START_DATA_BYTE 0x40
#define CMD(__cmdbyte) \
  i2c_write2(0x00, (__cmdbyte))

#define CMD2(__cmdbyte, __valuebyte) \
  i2c_write3(0x00, (__cmdbyte), (__valuebyte))

#define CMD3(__cmdbyte, __valuebyte0, __valuebyte1) \
  i2c_write4(0x00, (__cmdbyte), (__valuebyte0), (__valuebyte1))

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

#define SSD1306_SIZE_X 128
#define SSD1306_SIZE_Y 64
#define SSD1306_ROWS_PER_PAGE 8
#define SSD1306_NUM_PAGES (SSD1306_SIZE_Y / SSD1306_ROWS_PER_PAGE)

struct ssd1306_draw_io {
  char data_prefix;
  char buf[SSD1306_SIZE_X * SSD1306_NUM_PAGES];
} PACKED;

static struct ssd1306_draw_io ssd1306_io_buffer;

#define DBYTE(__col, __page) \
  (ssd1306_io_buffer.buf[__col + __page * SSD1306_SIZE_X])

void ssd1306_flush(void)
{
#if defined SSD1306_ADDRESSING_MODE_HORIZONTAL
  i2c_write_op(SSD1306_I2C_ADDR, (const uint8_t *)&ssd1306_io_buffer,
    sizeof(ssd1306_io_buffer),  true);
#elif defined SSD1306_ADDRESSING_MODE_PAGE
  int page;
  int col;
  /*
   * p and stored_byte are used to put 'reg' and 'data_bytes' into a single
   * buffer, so that it might be used in a single i2c byte sequence without
   * 'special' treatment for data/command byte.
   * Instead of sending out NUM_COLUMNS (128) bytes, we send 129 with first the
   * 0x40 bytes that indicate data start.
   *
   * This also simplifies things for DMA
   * This is then not so good for cacheing, so TODO: investigate how this
   * affects cacheing
   */
  CMD_SET_COL(0);

  for (page = 0; page < NUM_PAGES; ++page) {
    uint8_t *p;
    uint8_t stored_byte;

    CMD_SET_PAGE_START_ADDRESS(page);
    p = &DBYTE(0, page) - 1;
    stored_byte = *p;
    *p = START_DATA_BYTE;
    i2c_write_op(SSD1306_I2C_ADDR, p, NUM_COLUMNS + 1, true);
    *p = stored_byte;
  }
#endif
}

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

  i2c_write_op(SSD1306_I2C_ADDR, buf, sizeof(buf), false);
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

  i2c_write_op(SSD1306_I2C_ADDR, buf, sizeof(buf), false);
  CMD(0x2f);
}

void ssd1306_arg(uint16_t arg)
{
  CMD_SET_CONTRAST(arg >> 4);
}

#ifdef CNF_DSPL_PWR
void ssd1306_power_init(void)
{
  /*
   * Control display power with transistor. GPIO pin connected to transistor
   * base is specified int the config. Pin is configured as PUSH/PULL output
   * If power control is enabled, we first shut down the power for display,
   * then put it back on to reset possible interrupted i2c state. This is
   * needed if we reset device multiple times on debug while i2c transimission
   * was not completed by to some logical point, then I2C re-initialization
   * would be harder than it needs to be
   */
  rcc_periph_ena(RCC_GPIO_PORT(CNF_DSPL_PWR_PORT));

  gpio_setup(GPIO_PORT(CNF_DSPL_PWR_PORT), CNF_DSPL_PWR_PIN,
    GPIO_MODE_OUT_10_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);

  gpio_odr_modify(GPIO_PORT(CNF_DSPL_PWR_PORT), CNF_DSPL_PWR_PIN, false);
  wait_ms(100);
  gpio_odr_modify(GPIO_PORT(CNF_DSPL_PWR_PORT), CNF_DSPL_PWR_PIN, true);
  wait_ms(10);
}
#endif /* CNF_DSPL_PWR */

void ssd1306_init(void)
{
#ifdef CNF_DSPL_PWR
  ssd1306_power_init();
#endif /* CNF_DSPL_PWR */

  CMD_SET_MULTIPLEX_RATIO(0x3f);
  CMD_SET_DISPL_OFFSET(0);
  CMD_SET_START_LINE_ADDR(0);
  CMD_SET_SEGMENT_REMAP(1);
  CMD_SET_COM_SCAN_DIR(COM_SCAN_UP);
  CMD_SET_COM_PINS_HW_CONF(1, 0);
  CMD_SET_CONTRAST(0xef);
  CMD_DISPL_SET_ON_OFF(DISPL_OFF);
  CMD_SET_INVERTED(DISPL_INVERTED_OFF);
  CMD_SET_CLK_DIV_RATIO(15, 0);
  CMD_MEMORY_ADDRESSING(MEMORY_ADDRESSING_HORIZ);
  CMD_DISPL_SET_TURN_ON_BEHAVIOR(DISPL_ON_RESUME_RAM);
  CMD_SET_PRECHARGE_PERIOD(0x1, 0x1);
  CMD_SET_VCOM_DESELECT_LEVEL(1);
  CMD_CHARGE_PUMP_ENA();
  CMD_DISPL_SET_ON_OFF(DISPL_ON);

  ssd1306_io_buffer.data_prefix = START_DATA_BYTE;
  display_init(ssd1306_io_buffer.buf, SSD1306_SIZE_X, SSD1306_SIZE_Y,
    SSD1306_ROWS_PER_PAGE);
  display_clear();
  ssd1306_flush();
}

void ssd1306_get_size(int *size_x, int *size_y);
{
  *size_x = SSD1306_SIZE_X;
  *size_y = SSD1306_SIZE_Y;
}
