#include "spi.h"
#include "memory_layout.h"
#include "reg_access.h"
#include "gpio.h"

#define SPI_CR1 (reg16_t)(SPI1_BASE + 0x00)

#define SPI_CR1_CPHA 0
#define SPI_CR1_CPHA_WIDTH 1
#define SPI_CR1_CPOL 1
#define SPI_CR1_CPOL_WIDTH 1
#define SPI_CR1_MSTR 2
#define SPI_CR1_MSTR_WIDTH 1
#define SPI_CR1_BR 3
#define SPI_CR1_BR_WIDTH 3
#define SPI_CR1_SPE 6
#define SPI_CR1_SPE_WIDTH 1
#define SPI_CR1_LSBFIRST 7
#define SPI_CR1_LSBFIRST_WIDTH 1
#define SPI_CR1_SSI 8
#define SPI_CR1_SSI_WIDTH 1
#define SPI_CR1_SSM 9
#define SPI_CR1_SSM_WIDTH 1
#define SPI_CR1_RXONLY 10
#define SPI_CR1_RXONLY_WIDTH 1
#define SPI_CR1_DFF 11
#define SPI_CR1_DFF_WIDTH 1
#define SPI_CR1_CRCNEXT 12
#define SPI_CR1_CRCNEXT_WIDTH 1
#define SPI_CR1_CRCEN 13
#define SPI_CR1_CRCEN_WIDTH 1
#define SPI_CR1_BIDIOE 14
#define SPI_CR1_BIDIOE_WIDTH 1
#define SPI_CR1_BIDIMODE 15
#define SPI_CR1_BIDIMODE_WIDTH 1

#define SPI_CR2 (reg16_t)(SPI1_BASE + 0x04)
#define SPI_SR (reg16_t)(SPI1_BASE + 0x08)
#define SPI_SR_RXNE 0
#define SPI_SR_TXE 1
#define SPI_SR_CHSIDE 2
#define SPI_SR_UDR 3
#define SPI_SR_CRCERR 4
#define SPI_SR_MODF 5
#define SPI_SR_OVR 6
#define SPI_SR_BSY 7

#define SPI_DR (reg16_t)(SPI1_BASE + 0x0c)
#define SPI_CRCPR (reg16_t)(SPI1_BASE + 0x10)
#define SPI_RXCRCR (reg16_t)(SPI1_BASE + 0x14)
#define SPI_TXCRCR (reg16_t)(SPI1_BASE + 0x18)
#define SPI_I2SCFGR (reg16_t)(SPI1_BASE + 0x1c)
#define SPI_I2SPR (reg16_t)(SPI1_BASE + 0x20)

void spi_gpio_init(void)
{
  /* MISO /    master - NONE */
  /* NSS  / HW master - alternate function push-pull */
  gpio_setup(GPIO_PORT_A, 4, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);

  /* SCK  /    master - alternate function push-pull */
  gpio_setup(GPIO_PORT_A, 5, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
  /* MOSI /    master - alternate function push-pull */
  gpio_setup(GPIO_PORT_A, 7, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
}

void spi_init(void)
{
  spi_gpio_init();
  /* Enable Master mode */
  reg16_set_bit(SPI_CR1, SPI_CR1_MSTR);
  reg16_modify_bits(SPI_CR1, SPI_CR1_BR, SPI_CR1_BR_WIDTH, 3);
  reg16_clear_bit(SPI_CR1, SPI_CR1_LSBFIRST);
  reg16_set_bit(SPI_CR1, SPI_CR1_SSI);
  reg16_set_bit(SPI_CR1, SPI_CR1_SSM);
  reg16_clear_bit(SPI_CR1, SPI_CR1_DFF);
  reg_write16(SPI_CR2, 0);
  reg16_set_bit(SPI_CR1, SPI_CR1_SPE);
}

void spi_transfer_byte(uint8_t data)
{
  uint16_t sr;

  reg_write(SPI_DR, data);
  while(!reg16_bit_is_set(SPI_SR, SPI_SR_TXE));
  while(reg16_bit_is_set(SPI_SR, SPI_SR_BSY));
}


void spi_transfer(const uint8_t *data, int len)
{
  for (int i = 0; i < len; ++i) {
    spi_transfer_byte(data[i]);
  }
}
