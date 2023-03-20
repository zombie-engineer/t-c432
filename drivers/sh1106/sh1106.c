#include "sh1106.h"
#include "spi.h"
#include "gpio.h"

#define SH1106_CMD_DISPLAY_ON 0xaf
#define SH1106_CMD_DISPLAY_OFF 0xae
#define SH1106_CMD_SET_MULTIPLEX_RATIO 0xa8
#define SH1106_CMD_CLOCK_FREQ 0xd5
#define SH1106_CMD_SET_CHARGE_ADJ_TIME 0xd9
#define SH1106_CMD_SET_PAGE_ADDRESS 0xb0
#define SH1106_CMD_SET_LOW_COLUMN_ADDR 0x00
#define SH1106_CMD_SET_HIGH_COLUMN_ADDR 0x10

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

static void sh1106_data_one(uint8_t byte)
{
  uint8_t data;
  sh1106_dc_high();

  data = byte;
  spi_transfer(&data, 1);
  sh1106_dc_low();
}

void sh1106_flush(char pattern)
{
  for (int page = 0; page < 8; ++page) {
    sh1106_cmd(SH1106_CMD_SET_LOW_COLUMN_ADDR | 0);
    sh1106_cmd(SH1106_CMD_SET_HIGH_COLUMN_ADDR | 0);
    sh1106_cmd(SH1106_CMD_SET_PAGE_ADDRESS | page);
    for(int i = 0; i < 130; ++i) {
      sh1106_data_one(pattern);
      for (volatile int j = 0; j < 50000; ++j);
    }
  }
}

void sh1106_init(void)
{
  asm volatile ("bkpt");
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
  sh1106_cmd2(SH1106_CMD_CLOCK_FREQ, 0x00);
  sh1106_cmd2(SH1106_CMD_SET_CHARGE_ADJ_TIME, 0xff);

  while(1) {
    sh1106_flush(1);
    sh1106_flush(0xe);;
  }
  while(1);
}
