#include "sh1106.h"
#include "spi.h"
#include "gpio.h"
#include <display.h>
#include <compiler.h>
#include <string.h>

#define SH1106_CMD_DISPLAY_ON 0xaf
#define SH1106_CMD_DISPLAY_OFF 0xae
#define SH1106_CMD_SET_MULTIPLEX_RATIO 0xa8
#define SH1106_CMD_CLOCK_FREQ 0xd5
#define SH1106_CMD_SET_CHARGE_ADJ_TIME 0xd9
#define SH1106_CMD_SET_SCAN_DIR_NORMAL 0xc0
#define SH1106_CMD_SET_SCAN_DIR_FLIPPED 0xc8
#define SH1106_CMD_SET_PAGE_ADDRESS 0xb0
#define SH1106_CMD_SET_LOW_COLUMN_ADDR 0x00
#define SH1106_CMD_SET_HIGH_COLUMN_ADDR 0x10

#define SH1106_SIZE_X 132
#define SH1106_SIZE_Y 64
#define SH1106_NUM_PAGES 8

struct sh1106_draw_io {
  char data_prefix;
  char buf[SH1106_SIZE_X * SH1106_NUM_PAGES];
} PACKED;

struct sh1106_draw_io sh1106_io_buffer;

static void sh1106_cs_init(void)
{
  gpio_setup(GPIO_PORT_A, 4, GPIO_MODE_OUT_2_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);
}

static void sh1106_cs_high(void)
{
  gpio_odr_modify(GPIO_PORT_A, 4, 1);
}

static void sh1106_cs_low(void)
{
  gpio_odr_modify(GPIO_PORT_A, 4, 0);
}

static void sh1106_rst_init(void)
{
  gpio_setup(GPIO_PORT_A, 6, GPIO_MODE_OUT_2_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);
}

static void sh1106_rst_high(void)
{
  gpio_odr_modify(GPIO_PORT_A, 6, 1);
}

static void sh1106_rst_low(void)
{
  gpio_odr_modify(GPIO_PORT_A, 6, 0);
}

static void sh1106_dc_init(void)
{
  gpio_setup(GPIO_PORT_B, 0, GPIO_MODE_OUT_2_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);
}

static void sh1106_dc_high(void)
{
  gpio_odr_modify(GPIO_PORT_B, 0, 1);
}

static void sh1106_dc_low(void)
{
  gpio_odr_modify(GPIO_PORT_B, 0, 0);
}

static void sh1106_cmd(uint8_t byte)
{
  uint8_t data;
  sh1106_dc_low();

  data = byte;
  spi_transfer(&data, 1);
}

static void sh1106_cmd2(uint8_t byte1, uint8_t byte2)
{
  uint8_t data[] = { byte1, byte2 };
  sh1106_dc_low();

  spi_transfer(data, 2);
}

static void sh1106_flush_page(int page)
{
  sh1106_dc_high();
  spi_transfer(&sh1106_io_buffer.buf[SH1106_SIZE_X * page], SH1106_SIZE_X);
}

void sh1106_flush(void)
{
  sh1106_cs_low();
  for (int page = 0; page < 8; ++page) {
    sh1106_cmd(SH1106_CMD_SET_LOW_COLUMN_ADDR | 0);
    sh1106_cmd(SH1106_CMD_SET_HIGH_COLUMN_ADDR | 0);
    sh1106_cmd(SH1106_CMD_SET_PAGE_ADDRESS | page);
    sh1106_flush_page(page);
  }
  sh1106_cs_high();
}

void sh1106_init(void)
{
  spi_init();
  sh1106_cs_init();
  sh1106_rst_init();
  sh1106_dc_init();

  sh1106_cs_high();
  sh1106_rst_low();
  sh1106_rst_high();
  sh1106_cs_low();

  sh1106_cmd(SH1106_CMD_DISPLAY_ON);
  sh1106_cmd2(SH1106_CMD_SET_MULTIPLEX_RATIO, 63);
  sh1106_cmd2(SH1106_CMD_CLOCK_FREQ, 0xf0);
  sh1106_cmd2(SH1106_CMD_SET_CHARGE_ADJ_TIME, 0x11);
  sh1106_cmd(SH1106_CMD_SET_SCAN_DIR_FLIPPED);

  display_init(sh1106_io_buffer.buf, SH1106_SIZE_X, SH1106_SIZE_Y,
    SH1106_SIZE_Y / SH1106_NUM_PAGES);
  sh1106_cs_high();
}

void sh1106_get_size(int *size_x, int *size_y)
{
  *size_x = SH1106_SIZE_X;
  *size_y = SH1106_SIZE_Y;
}
