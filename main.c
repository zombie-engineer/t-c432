#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"


#define RCC_BASE 0x40021000
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
#define RCC_CFGR_HPRE_WIDTH 3
#define RCC_CFGR_PPRE1_POS 8
#define RCC_CFGR_PPRE1_WIDTH 3
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
#define RCC_CFGR_USBPRE_NO_DIV 0
#define RCC_CFGR_USBPRE_DIV 1
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
#define RCC_APB1ENR_TIM2EN 0
#define RCC_APB1RSTR_IOPCRST 4
#define RCC_APB1ENR_USART2EN 17
#define RCC_APB1ENR_USBEN 23

#define RCC_APB2ENR_IOPAEN 2
#define RCC_APB2ENR_IOPBEN 3
#define RCC_APB2ENR_IOPCEN 4
#define RCC_APB2ENR_ADC1AEN 9

#define USART_BASE 0x40004400
#define USART_SR  (volatile uint32_t *)(USART_BASE + 0x00)
#define USART_DR  (volatile uint32_t *)(USART_BASE + 0x04)
#define USART_BRR (volatile uint32_t *)(USART_BASE + 0x08)
#define USART_CR1 (volatile uint32_t *)(USART_BASE + 0x0c)

#define IOPC_BASE 0x40011000
#define GPIOC_CRL  (volatile uint32_t *)(IOPC_BASE + 0x00)
#define GPIOC_CRH  (volatile uint32_t *)(IOPC_BASE + 0x04)
#define GPIOC_IDR  (volatile uint32_t *)(IOPC_BASE + 0x08)
#define GPIOC_ODR  (volatile uint32_t *)(IOPC_BASE + 0x0c)
#define GPIOC_BSRR (volatile uint32_t *)(IOPC_BASE + 0x10)
#define GPIOC_BRR  (volatile uint32_t *)(IOPC_BASE + 0x14)
#define GPIOC_LCKR (volatile uint32_t *)(IOPC_BASE + 0x14)

#define IOPA_BASE 0x40010800
#define GPIOA_CRL  (volatile uint32_t *)(IOPA_BASE + 0x00)
#define GPIOA_CRH  (volatile uint32_t *)(IOPA_BASE + 0x04)
#define GPIOA_IDR  (volatile uint32_t *)(IOPA_BASE + 0x08)
#define GPIOA_ODR  (volatile uint32_t *)(IOPA_BASE + 0x0c)
#define GPIOA_BSRR (volatile uint32_t *)(IOPA_BASE + 0x10)
#define GPIOA_BRR  (volatile uint32_t *)(IOPA_BASE + 0x14)
#define GPIOA_LCKR (volatile uint32_t *)(IOPA_BASE + 0x14)

#define AFIO_BASE 0x40010000
#define AFIO_EVCR (volatile uint32_t *)(AFIO_BASE + 0x00)
#define AFIO_MAPR (volatile uint32_t *)(AFIO_BASE + 0x04)

#define TIM2_BASE 0x40000000

#define TIM2_CR1   (volatile uint32_t *)(TIM2_BASE + 0x00)
#define TIM2_CR2   (volatile uint32_t *)(TIM2_BASE + 0x04)
#define TIM2_SMCR  (volatile uint32_t *)(TIM2_BASE + 0x08)
#define TIM2_DIER  (volatile uint32_t *)(TIM2_BASE + 0x0c)
#define TIM2_SR    (volatile uint32_t *)(TIM2_BASE + 0x10)
#define TIM2_EGR   (volatile uint32_t *)(TIM2_BASE + 0x14)
#define TIM2_CCMR1 (volatile uint32_t *)(TIM2_BASE + 0x18)
#define TIM2_CCMR2 (volatile uint32_t *)(TIM2_BASE + 0x1c)
#define TIM2_CCER  (volatile uint32_t *)(TIM2_BASE + 0x20)
#define TIM2_TCNT  (volatile uint32_t *)(TIM2_BASE + 0x24)
#define TIM2_PSC   (volatile uint32_t *)(TIM2_BASE + 0x28)
#define TIM2_ARR   (volatile uint32_t *)(TIM2_BASE + 0x2c)
#define TIM2_CCR1  (volatile uint32_t *)(TIM2_BASE + 0x34)
#define TIM2_CCR2  (volatile uint32_t *)(TIM2_BASE + 0x38)
#define TIM2_CCR3  (volatile uint32_t *)(TIM2_BASE + 0x3c)
#define TIM2_CCR4  (volatile uint32_t *)(TIM2_BASE + 0x40)
#define TIM2_DCR   (volatile uint32_t *)(TIM2_BASE + 0x48)
#define TIM2_DMAR  (volatile uint32_t *)(TIM2_BASE + 0x4c)

#define NVIC_BASE 0xe000e100
/*
 * Interrupt set-enable registers.
 * On read shows which interrupts are enabled.
 * On writing 1 enables interrupt.
 */
#define NVIC_ISER0 (volatile uint32_t *)(NVIC_BASE + 0x00)
#define NVIC_ISER1 (volatile uint32_t *)(NVIC_BASE + 0x04)
#define NVIC_ISER2 (volatile uint32_t *)(NVIC_BASE + 0x08)

/*
 * Interrupt clear-enable registers.
 * On read shows which interrupts are enabled.
 * On writing 1 disables interrupt.
 */
#define NVIC_ICER0 (volatile uint32_t *)(NVIC_BASE + 0x80)
#define NVIC_ICER1 (volatile uint32_t *)(NVIC_BASE + 0x84)
#define NVIC_ICER2 (volatile uint32_t *)(NVIC_BASE + 0x88)

/*
 * Interrupt set-pending registers.
 * On read shows which interrupts are pending.
 * On writing 1 set pending status.
 */
#define NVIC_ISPR0 (volatile uint32_t *)(NVIC_BASE + 0x100)
#define NVIC_ISPR1 (volatile uint32_t *)(NVIC_BASE + 0x104)
#define NVIC_ISPR2 (volatile uint32_t *)(NVIC_BASE + 0x108)

/*
 * Interrupt clear-pending registers.
 * On read shows which interrupts are pending.
 * On writing 1 clears pending status.
 */
#define NVIC_ICPR0 (volatile uint32_t *)(NVIC_BASE + 0x180)
#define NVIC_ICPR1 (volatile uint32_t *)(NVIC_BASE + 0x184)
#define NVIC_ICPR2 (volatile uint32_t *)(NVIC_BASE + 0x188)

/* Interrupt active bits registers */
#define NVIC_IABR0 (volatile uint32_t *)(NVIC_BASE + 0x200)
#define NVIC_IABR1 (volatile uint32_t *)(NVIC_BASE + 0x204)
#define NVIC_IABR2 (volatile uint32_t *)(NVIC_BASE + 0x208)

/* Interrupt priority registers */
#define NVIC_IPR0 (volatile uint32_t *)(NVIC_BASE + 0x300)
#define NVIC_STIR (volatile uint32_t *)(NVIC_BASE + 0xe00)


#define ADC1_BASE 0x40012400
#define ADC1_SR    (volatile uint32_t *)(ADC1_BASE + 0x00)
#define ADC1_CR1   (volatile uint32_t *)(ADC1_BASE + 0x04)
#define ADC1_CR2   (volatile uint32_t *)(ADC1_BASE + 0x08)
#define ADC1_SMPR1 (volatile uint32_t *)(ADC1_BASE + 0x0c)
#define ADC1_SMPR2 (volatile uint32_t *)(ADC1_BASE + 0x10)
#define ADC1_JOFR0 (volatile uint32_t *)(ADC1_BASE + 0x14)
#define ADC1_JOFR1 (volatile uint32_t *)(ADC1_BASE + 0x18)
#define ADC1_JOFR2 (volatile uint32_t *)(ADC1_BASE + 0x1c)
#define ADC1_JOFR3 (volatile uint32_t *)(ADC1_BASE + 0x20)
#define ADC1_HTR   (volatile uint32_t *)(ADC1_BASE + 0x24)
#define ADC1_LTR   (volatile uint32_t *)(ADC1_BASE + 0x28)
#define ADC1_JSQR  (volatile uint32_t *)(ADC1_BASE + 0x2c)
#define ADC1_JDR0  (volatile uint32_t *)(ADC1_BASE + 0x3c)
#define ADC1_JDR1  (volatile uint32_t *)(ADC1_BASE + 0x40)
#define ADC1_JDR2  (volatile uint32_t *)(ADC1_BASE + 0x44)
#define ADC1_JDR3  (volatile uint32_t *)(ADC1_BASE + 0x48)
#define ADC1_DR    (volatile uint32_t *)(ADC1_BASE + 0x4c)

#define ADC_CR2_EON 0
#define ADC_CR1_EOCIE 5

#define USB_BASE 0x40005c00
#define USB_EP0R (volatile uint32_t *)(USB_BASE + 0x0000)
#define USB_EPXR_EA 0
#define USB_EPXR_EA_WIDTH 4
#define USB_EPXR_STAT_TX 4
#define USB_EPXR_STAT_TX_WIDTH 2
#define USB_EPXR_DTOG_TX 6
#define USB_EPXR_DTOG_TX_WIDTH 1
#define USB_EPXR_CTR_TX 7
#define USB_EPXR_CTR_TX_WIDTH 1
#define USB_EPXR_EP_KIND 8
#define USB_EPXR_EP_KIND_WIDTH 1
#define USB_EPXR_EP_TYPE 9
#define USB_EPXR_EP_TYPE_WIDTH 2
#define USB_EPXR_SETUP 11
#define USB_EPXR_SETUP_WIDTH 1
#define USB_EPXR_STAT_RX 12
#define USB_EPXR_STAT_RX_WIDTH 1
#define USB_EPXR_DTOG_RX 14
#define USB_EPXR_DTOG_RX_WIDTH 1
#define USB_EPXR_CTR_RX 15
#define USB_EPXR_CTR_RX_WIDTH 1

#define USB_EP1R (volatile uint32_t *)(USB_BASE + 0x0004)
#define USB_EP2R (volatile uint32_t *)(USB_BASE + 0x0008)
#define USB_EP3R (volatile uint32_t *)(USB_BASE + 0x000c)
#define USB_EP4R (volatile uint32_t *)(USB_BASE + 0x0010)
#define USB_EP5R (volatile uint32_t *)(USB_BASE + 0x0014)
#define USB_EP6R (volatile uint32_t *)(USB_BASE + 0x0018)
#define USB_EP7R (volatile uint32_t *)(USB_BASE + 0x001c)

#define USB_CNTR (volatile uint32_t *)(USB_BASE + 0x0040)
#define USB_ISTR (volatile uint32_t *)(USB_BASE + 0x0044)
#define USB_FNR (volatile uint32_t *)(USB_BASE + 0x0048)
#define USB_FNR_FN 0
#define USB_FNR_FN_WIDTH 11
#define USB_FNR_LSOF 11
#define USB_FNR_LSOF_WIDTH 2
#define USB_FNR_LCK 13
#define USB_FNR_LSOF_WIDTH 1
#define USB_FNR_RXDM 14
#define USB_FNR_RXDM_WIDTH 1
#define USB_FNR_RXDP 15
#define USB_FNR_RXDP_WIDTH 1
#define USB_DADDR (volatile uint32_t *)(USB_BASE + 0x004c)
#define USB_BTABLE (volatile uint32_t *)(USB_BASE + 0x0050)

#define USB_CNTR_FRES 0
#define USB_CNTR_PDWN 1
#define USB_CNTR_LPMODE 2
#define USB_CNTR_FSUSP 3
#define USB_CNTR_RESUME 4
#define USB_CNTR_ESOFM 8
#define USB_CNTR_SOFM 9
#define USB_CNTR_RESETM 10
#define USB_CNTR_SUSPM 11
#define USB_CNTR_WKUPM 12
#define USB_CNTR_ERRM 13
#define USB_CNTR_PMAOVRNM 14
#define USB_CNTR_CTRM 15

#define USB_ISTR_DIR 4
#define USB_ISTR_ESOF 8
#define USB_ISTR_SOF 9
#define USB_ISTR_RESET 10
#define USB_ISTR_SUSP 11
#define USB_ISTR_WKUP 12
#define USB_ISTR_ERR 13
#define USB_ISTR_PMAOVRN 14
#define USB_ISTR_CTR 15

#define THUMB __attribute__((target("thumb")))

#define F_CLK 8000000

static void gpioc_bit_set(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1<<pin_nr);
}

static void gpioc_bit_clear(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1 << (pin_nr + 16));
}

static void gpiox_set_cr(uint32_t basereg, int pin_nr, int mode, int cnf)
{
  uint32_t v;
  int b;
  uint32_t addr;

  addr = basereg;
  addr += (pin_nr / 8) * 4;
  b = (pin_nr % 8) * 4;

  v = reg_read(addr);
  u32_modify_bits(&v, b, 2, mode);
  u32_modify_bits(&v, b + 2, 2, cnf);
  reg_write(addr, v);
}

static void gpioc_set_cr(int pin_nr, int mode, int cnf)
{
  gpiox_set_cr(GPIOC_CRL, pin_nr, mode, cnf);
}

static void gpioa_set_cr(int pin_nr, int mode, int cnf)
{
  gpiox_set_cr(GPIOA_CRL, pin_nr, mode, cnf);
}

static void gpioc_set_pin13(void)
{
  gpioc_set_cr(13, 1, 1);
  gpioc_bit_set(13);
  gpioc_bit_clear(13);
}

typedef enum {
  RCC_SRC_HSI,
  RCC_SRC_HSE,
  RCC_SRC_PLL,
} rcc_src_enum;

#define TIMx_CEN  (1<<0)
#define TIMx_UDIS (1<<1)
#define TIMx_URS  (1<<2)
#define TIMx_OPM  (1<<3)
#define TIMx_DIR  (1<<4)
#define TIMx_CMS  (3<<5)
#define TIMx_ARPE (1<<7)
#define TIMx_CKD  (3<<8)

#define NVIC_INTERRUPT_NUMBER_ADC1 18
#define NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX 19
#define NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0 20
#define NVIC_INTERRUPT_NUMBER_TIM2 28
#define NVIC_INTERRUPT_NUMBER_USB_WAKEUP 42

#if 0
uint16_t tim_calc_psc(float timeout, uint32_t f_clk, uint16_t auto_reload_value)
{
  return ;
}
#endif

#define CALC_PSC(timeout, f_clk, auto_reload_value) (uint16_t)((float)f_clk * timeout / auto_reload_value - 1.0f)
static int tim2_setup(
  bool one_pulse,
  uint16_t prescaler,
  uint16_t counter_value,
  bool enable_interrupt,
  bool enable)
{
  uint16_t v;

  reg_write(TIM2_ARR, counter_value);
  reg_write(TIM2_PSC, prescaler);

  v = reg_read(TIM2_CR1);

  /* Auto-reload */
  v |= TIMx_ARPE;

  /* center-aligned off */
  v &= ~TIMx_CMS;

  /* count dir = up */
  v &= ~TIMx_DIR;

  if (one_pulse)
    v |= TIMx_OPM;
  else
    v &= ~TIMx_OPM;

  /* Update only from overflow */
  v |= TIMx_URS;

  /* Enable interrupt for TIM2 */
  if (enable_interrupt) {
    /* Event on overflow enabled */
    reg_write(TIM2_SR, 0);
    reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
    v &= TIMx_UDIS;
    reg_write(TIM2_DIER, 1);
    reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
  }

  if (enable) {
    v |= TIMx_CEN;;
  }

  reg_write(TIM2_CR1, v);
  
  return 0;
}

static volatile int last_adc = 0;

void adc_isr(void)
{
  last_adc = reg_read(ADC1_DR);
  reg_write(ADC1_SR, 0);
  reg_write(ADC1_CR2, 1 << ADC_CR2_EON);
}

void tim2_isr(void)
{
  static int toggle_flag = 0;
  reg_write(TIM2_ARR, last_adc + 600);
  reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
  reg_write(TIM2_SR, 0);
  reg32_set_bit(TIM2_CR1, 0);

  if (toggle_flag) {
    gpioc_bit_set(13);
    toggle_flag = 0;
  } else {
    gpioc_bit_clear(13);
    toggle_flag = 1;
  }
}

void adc_setup(void)
{
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_ADC1AEN);
  gpioa_set_cr(1, 0, 0);

  reg_write(ADC1_CR2, 1 << ADC_CR2_EON);
  reg32_set_bit(ADC1_CR1, ADC_CR1_EOCIE);
  reg_write(ADC1_SR, 0);
  reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_ADC1);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_ADC1);
  for (volatile int i = 0; i < 400; ++i)
  {
  }
  reg_write(ADC1_CR2, 1 << ADC_CR2_EON);
}

void uart2_setup(void)
{
  uint32_t v;
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_USART2EN);

  gpioa_set_cr(2, 3, 2);
  gpioa_set_cr(3, 0, 2);
  reg32_set_bit(GPIOA_ODR, 3);
  v = reg_read(AFIO_MAPR);
  reg_write(USART_CR1, 0);
  reg_write(USART_CR1, 1<<13);
  reg_write(USART_BRR, 5 | (4 << 4));
  reg_write(USART_CR1, (1<<13) | 0xc);
  while(1) {
    reg_write(USART_DR, (uint32_t) 0 | 'c');
  }
}

void timer_setup(void)
{
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_TIM2EN);
  reg32_set_bit(RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
  gpioc_set_pin13();
  tim2_setup(true, CALC_PSC(5.0f, F_CLK, 0xffff), 0xffff, true, true);
}

#define FLASH_BASE 0x40022000
#define FLASH_ACR (volatile uint32_t *)(FLASH_BASE + 0x00)

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
 reg_write(FLASH_ACR, 0x12);

  /*
   * Configure PLL to HSE (8Hz) * 9 = 72MHz 
   * USB is configured to /1.5 = 48MHz
   */
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_PLLSRC_POS, RCC_CFGR_PLLSRC_WIDTH, RCC_CFGR_PLLSRC_HSE);
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_PLLMUL_POS, RCC_CFGR_PLLMUL_WIDTH, RCC_CFGR_PLLMUL_X9);
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_USBPRE_POS, RCC_CFGR_USBPRE_WIDTH, RCC_CFGR_USBPRE_NO_DIV);

  /* Turn on PLL, after that both PLL + HSE will be running.*/
  reg32_set_bit(RCC_CR, RCC_CR_PLLON_POS);
  while(!reg32_bit_is_set(RCC_CR, RCC_CR_PLLRDY_POS));

  /* Select PLL as clock source */
  reg32_modify_bits(RCC_CFGR, RCC_CFGR_SW_POS, RCC_CFGR_SW_WIDTH, RCC_CFGR_SW_PLL);
  while(!reg32_bits_eq(RCC_CFGR, RCC_CFGR_SWS_POS, RCC_CFGR_SWS_WIDTH, RCC_CFGR_SW_PLL));

  reg32_clear_bit(RCC_CR, RCC_CR_HSION_POS);
  while(reg32_bit_is_set(RCC_CR, RCC_CR_HSIRDY_POS));
}

void usb_hp_isr(void)
{
}

void usb_lp_isr(void)
{
}

void usb_wakeup_isr(void)
{
}

void usb_setup(void)
{
  rcc_set_72mhz_usb();
  reg32_set_bit(RCC_APB1ENR, RCC_APB1ENR_USBEN);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0);
  reg_write(NVIC_ISER1, 1 << NVIC_INTERRUPT_NUMBER_USB_WAKEUP - 32);
  uint32_t v = reg_read(USB_CNTR);
  reg32_clear_bit(USB_CNTR, USB_CNTR_FRES);
  if (v);
}

extern uint32_t __bss_start;
extern uint32_t __bss_end;

void zero_bss(void)
{
  for (uint32_t *p = &__bss_start; p < &__bss_end; p++) {
    *p = 0;
  }
}

void main(void)
{
  usb_setup();
  zero_bss();

  timer_setup();
//  uart2_setup();
  adc_setup();
  while(1);
}
