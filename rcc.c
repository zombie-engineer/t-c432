#include "rcc.h"
#include "flash.h"
#include "memory_layout.h"
#include "reg_access.h"

#define RCC_CR      (volatile uint32_t *)(RCC_BASE + 0x00)
#define RCC_CFGR    (volatile uint32_t *)(RCC_BASE + 0x04)
#define RCC_APB2RSTR (volatile uint32_t *)(RCC_BASE + 0x0c)
#define RCC_APB1RSTR (volatile uint32_t *)(RCC_BASE + 0x10)
#define RCC_AHBENR   (volatile uint32_t *)(RCC_BASE + 0x14)
#define RCC_APB2ENR  (volatile uint32_t *)(RCC_BASE + 0x18)
#define RCC_APB1ENR  (volatile uint32_t *)(RCC_BASE + 0x1c)

#define RCC_CFGR_SW_POS 0
#define RCC_CFGR_SW_WIDTH 2

#define RCC_CFGR_SW_HSI  0b00
#define RCC_CFGR_SW_HSE  0b01
#define RCC_CFGR_SW_PLL  0b10

#define RCC_CFGR_SWS_POS 2
#define RCC_CFGR_SWS_WIDTH 2

#define RCC_CFGR_SWS_HSI  0b0000
#define RCC_CFGR_SWS_HSE  0b0100
#define RCC_CFGR_SWS_PLL  0b1000
#define RCC_CFGR_SWS_MASK 0b1100

#define RCC_CFGR_HPRE_POS 4
#define RCC_CFGR_HPRE_WIDTH 4
#define RCC_CFGR_HPRE_BY_1  0b0000
#define RCC_CFGR_HPRE_BY_2  0b1000
#define RCC_CFGR_HPRE_BY_4  0b1001
#define RCC_CFGR_HPRE_BY_8  0b1010
#define RCC_CFGR_HPRE_BY_16 0b1011
#define RCC_CFGR_HPRE_BY_64 0b1100
#define RCC_CFGR_HPRE_BY_128 0b1101
#define RCC_CFGR_HPRE_BY_256 0b1110
#define RCC_CFGR_HPRE_BY_512 0b1111

#define RCC_CFGR_PPRE1_POS 8
#define RCC_CFGR_PPRE1_WIDTH 3
#define RCC_CFGR_PPRE1_DIV_1 0b000
#define RCC_CFGR_PPRE1_DIV_2 0b100
#define RCC_CFGR_PPRE1_DIV_4 0b101
#define RCC_CFGR_PPRE1_DIV_8 0b110
#define RCC_CFGR_PPRE1_DIV_16 0b111

#define RCC_CFGR_PPRE2_POS 11
#define RCC_CFGR_PPRE2_WIDTH 3

#define RCC_CFGR_ADCPRE_POS 14
#define RCC_CFGR_ADCPRE_WIDTH 2

#define RCC_CFGR_PLLSRC_POS 16
#define RCC_CFGR_PLLSRC_WIDTH 1
#define RCC_CFGR_PLLSRC_HSI_DIV_2 0
#define RCC_CFGR_PLLSRC_HSE 1

#define RCC_CFGR_PLLXTRPE_POS 17
#define RCC_CFGR_PLLMUL_POS 18
#define RCC_CFGR_PLLMUL_WIDTH 4
#define RCC_CFGR_PLLMUL_X2 0
#define RCC_CFGR_PLLMUL_X3 1
#define RCC_CFGR_PLLMUL_X4 2
#define RCC_CFGR_PLLMUL_X5 3
#define RCC_CFGR_PLLMUL_X6 4
#define RCC_CFGR_PLLMUL_X7 5
#define RCC_CFGR_PLLMUL_X8 6
#define RCC_CFGR_PLLMUL_X9 7
#define RCC_CFGR_PLLMUL_X10 8
#define RCC_CFGR_PLLMUL_X11 9
#define RCC_CFGR_PLLMUL_X12 10
#define RCC_CFGR_PLLMUL_X13 11
#define RCC_CFGR_PLLMUL_X14 12
#define RCC_CFGR_PLLMUL_X15 13
#define RCC_CFGR_PLLMUL_X16 14
#define RCC_CFGR_PLLMUL_X16A 16

#define RCC_CFGR_USBPRE_POS 22
#define RCC_CFGR_USBPRE_WIDTH 1
#define RCC_CFGR_USBPRE_DIV_1_5 0
#define RCC_CFGR_USBPRE_NO_DIV 1

#define RCC_CFGR_MCO_POS 24
#define RCC_CFGR_MCO_WIDTH 3

#define RCC_CR_HSION_POS 0
#define RCC_CR_HSION_WIDTH 1
#define RCC_CR_HSIRDY_POS 1
#define RCC_CR_HSIRDY_WIDTH 1
#define RCC_CR_HSEON_POS 16
#define RCC_CR_HSEON_WIDTH 1
#define RCC_CR_HSERDY_POS 17
#define RCC_CR_HSERDY_WIDTH 1
#define RCC_CR_PLLON_POS 24
#define RCC_CR_PLLON_WIDTH 1
#define RCC_CR_PLLRDY_POS 25
#define RCC_CR_PLLRDY_WIDTH 1

#define RCC_APB1RSTR_TIM2RST 0
#define RCC_APB1RSTR_IOPCRST 4

#define RCC_APB1ENR_TIM2EN 0
#define RCC_APB1ENR_USART2EN 17
#define RCC_APB1ENR_I2C1EN 21
#define RCC_APB1ENR_I2C2EN 22
#define RCC_APB1ENR_USBEN 23

#define RCC_APB2ENR_AFIOEN 0
#define RCC_APB2ENR_IOPAEN 2
#define RCC_APB2ENR_IOPBEN 3
#define RCC_APB2ENR_IOPCEN 4
#define RCC_APB2ENR_ADC1AEN 9

void rcc_set_72mhz_usb(void)
{
  /* Enable HSE */
  reg32_set_bit(RCC_CR, RCC_CR_HSEON_POS);
  while(!reg32_bit_is_set(RCC_CR, RCC_CR_HSERDY_POS));

  /*
   * Prefetch buffer needs to be on when complex PLL clock math takes place
   * if prefetch buffer is not enabled, setting high speed clock (>24Mhz) will
   * result in errors during reading instructions from flash memory.
   */
  flash_enable_prefetch();

  /*
   * Configure PLL to HSE (8Hz) * 9 = 72MHz 
   * PLLCLK = HSE x 9 = 8MHz * 9 = 72MHz
   * SYSCLK = PLLCLK = 72MHz
   * HCLK   = SYSCLK / HPRE = 72MHz / 1 = 72MHz
   * PCLK1  = HCLK / PPRE1 = 72MHz / 2 = 36MHz
   * PCLK2  = HCLK / PPRE2 = 72MHz / 1 = 72MHz
   * ADCCLK = PCLK2 / ADCPRE = 72MHz / 1 = 72MHz
   * USB    = PLLCLK / USBPRE = 72MHZ / 1.5 = 48HMz
   */
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_PLLSRC_POS, RCC_CFGR_PLLSRC_WIDTH, RCC_CFGR_PLLSRC_HSE);
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_PLLMUL_POS, RCC_CFGR_PLLMUL_WIDTH, RCC_CFGR_PLLMUL_X9);
  //reg32_modify_bits(RCC_CFGR, RCC_CFGR_HPRE_POS, RCC_CFGR_HPRE_WIDTH, RCC_CFGR_HPRE_BY_1);
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_PPRE1_POS, RCC_CFGR_PPRE1_WIDTH, RCC_CFGR_PPRE1_DIV_2);

  reg32_modify_bits(RCC_CFGR, RCC_CFGR_USBPRE_POS, RCC_CFGR_USBPRE_WIDTH, RCC_CFGR_USBPRE_DIV_1_5);

  /* Turn on PLL, after that both PLL + HSE will be running.*/
  reg32_set_bit(RCC_CR, RCC_CR_PLLON_POS);
  while(!reg32_bit_is_set(RCC_CR, RCC_CR_PLLRDY_POS));

  /* Select PLL as clock source */
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_SW_POS, RCC_CFGR_SW_WIDTH, RCC_CFGR_SW_PLL);
  while(!reg32_bits_eq(RCC_CFGR, RCC_CFGR_SWS_POS, RCC_CFGR_SWS_WIDTH, RCC_CFGR_SW_PLL));

//  reg32_clear_bit(RCC_CR, RCC_CR_HSION_POS);
//  while(reg32_bit_is_set(RCC_CR, RCC_CR_HSIRDY_POS));
}

void rcc_enable_usb(void)
{
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_USBEN);
}

void rcc_enable_gpio_a(void)
{
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
}

void rcc_enable_gpio_b(void)
{
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
}

void rcc_enable_gpio_c(void)
{
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
}

void rcc_enable_afio(void)
{
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_AFIOEN);
}

void rcc_enable_adc1(void)
{
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_ADC1AEN);
}

void rcc_enable_usart2(void)
{
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_USART2EN);
}

void rcc_enable_tim2(void)
{
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_TIM2EN);
}

void rcc_enable_i2c1(void)
{
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_I2C1EN);
}

void rcc_enable_i2c2(void)
{
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_I2C2EN);
}
