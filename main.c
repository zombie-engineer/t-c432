#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"
#include "usb.h"
#include "rcc.h"

#define SYSTICK_BASE 0xe000e010
#define STK_CTRL (volatile uint32_t *)(SYSTICK_BASE + 0x00)
#define STK_CTRL_ENABLE 0
#define STK_CTRL_ENABLE_WIDTH 1
#define STK_CTRL_TICKINT 1
#define STK_CTRL_TICKINT_WIDTH 1
#define STK_CTRL_CLKSOURCE 2
#define STK_CTRL_CLKSOURCE_WIDTH 1
#define STK_CTRL_COUNTFLAG 16
#define STK_CTRL_COUNTFLAG_WIDTH 1

#define STK_LOAD (volatile uint32_t *)(SYSTICK_BASE + 0x04)
#define STK_VAL  (volatile uint32_t *)(SYSTICK_BASE + 0x08)
#define STK_CALIB (volatile uint32_t *)(SYSTICK_BASE + 0x0c)


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
#define USB_EPXR_STAT_TX_DISABLED 0b00
#define USB_EPXR_STAT_TX_STALL    0b01
#define USB_EPXR_STAT_TX_NAK      0b10
#define USB_EPXR_STAT_TX_VALID    0b11

#define USB_EPXR_DTOG_TX 6
#define USB_EPXR_DTOG_TX_WIDTH 1
#define USB_EPXR_CTR_TX 7
#define USB_EPXR_CTR_TX_WIDTH 1
#define USB_EPXR_EP_KIND 8
#define USB_EPXR_EP_KIND_WIDTH 1
#define USB_EPXR_EP_TYPE 9
#define USB_EPXR_EP_TYPE_WIDTH 2
#define USB_EPXR_EP_TYPE_BULK 0b00
#define USB_EPXR_EP_TYPE_CONTROL 0b01
#define USB_EPXR_EP_TYPE_ISO 0b10
#define USB_EPXR_EP_TYPE_INTERRUPT 0b11

#define USB_EPXR_SETUP 11
#define USB_EPXR_SETUP_WIDTH 1
#define USB_EPXR_STAT_RX 12
#define USB_EPXR_STAT_RX_WIDTH 2
#define USB_EPXR_STAT_RX_DISABLED 0b00
#define USB_EPXR_STAT_RX_STALL    0b01
#define USB_EPXR_STAT_RX_NAK      0b10
#define USB_EPXR_STAT_RX_VALID    0b11

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
#define USB_FNR_LCK_WIDTH 1
#define USB_FNR_RXDM 14
#define USB_FNR_RXDM_WIDTH 1
#define USB_FNR_RXDP 15
#define USB_FNR_RXDP_WIDTH 1
#define USB_DADDR (volatile uint32_t *)(USB_BASE + 0x004c)
#define USB_DADDR_ADDR 0
#define USB_DADDR_ADDR_WIDTH 7
#define USB_DADDR_EF 7

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

#define USB_ISTR_EP_ID 0
#define USB_ISTR_EP_ID_WIDTH 4
#define USB_ISTR_DIR 4
#define USB_ISTR_DIR_WIDTH 1
#define USB_ISTR_ESOF 8
#define USB_ISTR_SOF 9
#define USB_ISTR_RESET 10
#define USB_ISTR_SUSP 11
#define USB_ISTR_WKUP 12
#define USB_ISTR_ERR 13
#define USB_ISTR_PMAOVRN 14
#define USB_ISTR_CTR 15

#define USB_RAM (volatile uint32_t *)0x40006000
#define PMA_TO_SRAM_ADDR(__pma_addr) (USB_RAM + (__pma_addr) / 2)
#define SRAM_TO_PMA_ADDR(__sram_addr) ((uint32_t)(__sram_addr) - (uint32_t)(USB_RAM) / 2)

#define THUMB __attribute__((target("thumb")))

#define F_CLK 36000000

struct ep_buf_desc {
  uint16_t tx_addr;
  uint16_t tx_count;
  uint16_t rx_addr;
  uint16_t rx_count;
};

static void memcpy_pma_to_sram(void *dst, uint32_t pma_offset, int num_bytes)
{
  const volatile uint32_t *from = PMA_TO_SRAM_ADDR(pma_offset);
  /* We are reading 4 bytes, extract only lower 2-byte part of it */
  int i;
  for (i = 0; i < num_bytes / 2; i++) {
    ((uint16_t *)dst)[i] = from[i] & 0xffff;
  }
}

static void memcpy_sram_to_pma(uint32_t pma_dest, const void *src, int num_bytes)
{
  int i;
  volatile uint32_t *to = PMA_TO_SRAM_ADDR(pma_dest);
  const uint16_t *from = (const uint16_t *)src;
  for (i = 0; i < num_bytes / 2; i++) {
    to[i] = (uint32_t)from[i];
  }
}

static void gpioc_bit_set(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1<<pin_nr);
}

static void gpioc_bit_clear(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1 << (pin_nr + 16));
}

static void gpiox_set_cr(volatile uint32_t *basereg, int pin_nr, int mode, int cnf)
{
  uint32_t v;
  int b;
  volatile uint32_t *addr;

  addr = basereg;
  addr += (pin_nr / 8) * (4/4);
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
  // reg_write(TIM2_ARR, last_adc + 600);
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
  rcc_enable_gpio_a();
  rcc_enable_adc1();
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
  rcc_enable_gpio_a();
  rcc_enable_usart2();

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
  /* SYSCLK = 72MHz */
  rcc_enable_tim2();
  rcc_enable_gpio_c();
  gpioc_set_pin13();
  tim2_setup(true, CALC_PSC(0.5, F_CLK, 0xffff), 0xffff, true, true);
}


void usb_hp_isr(void)
{
  while(1);
}

struct usb_interrupt_stats {
  int num_sofs;
  int num_esofs;
  int num_errs;
  int num_susp;
  int num_resets;
  int num_transactions;
};

static struct usb_interrupt_stats usbstats = { 0 };

static void usb_sof_handler(void)
{
  usbstats.num_sofs++;
}

static void usb_esof_handler(void)
{
  usbstats.num_esofs++;
}

static void usb_reset_clear_ram(void)
{
  volatile uint32_t *p = USB_RAM;
  for (int i = 0; i < 256; ++i)
  {
    *p++ = 0;
  }
}

static void pma_init_bdt(void)
{
  uint16_t next;
  struct ep_buf_desc ebdt[8];
  next = sizeof(ebdt);

  for (int i = 0; i < 8; ++i) {
    ebdt[i].tx_addr = next;
    ebdt[i].tx_count = 0;
    next += 64;
    ebdt[i].rx_addr = next;
    ebdt[i].rx_count = (1<<15) | 1 << 10;
    next += 64;
  }
  memcpy_sram_to_pma(reg_read(USB_BTABLE), ebdt, sizeof(ebdt));
}

static void usb_ep_init(void)
{
  uint32_t v = 0;
  v |= USB_EPXR_EP_TYPE_CONTROL << USB_EPXR_EP_TYPE;
  v |= USB_EPXR_EP_TYPE_CONTROL << USB_EPXR_EP_KIND;
  v |= USB_EPXR_STAT_TX_STALL << USB_EPXR_STAT_TX;
  v |= USB_EPXR_STAT_RX_VALID << USB_EPXR_STAT_RX;
  reg_write(USB_EP0R, v);
}

static void usb_reset_handler(void)
{
  uint32_t v;
  usbstats.num_resets++;

  usb_reset_clear_ram();
  pma_init_bdt();
  reg_write(USB_BTABLE, 0);
  usb_ep_init();
  v = 0;
  u32_set_bit(&v, USB_DADDR_EF);
  reg_write(USB_DADDR, v);
}

static void usb_susp_handler(void)
{
  usbstats.num_susp++;
}

static void bp(void)
{
}

static void usb_err_handler(void)
{
  usbstats.num_errs++;
}

struct usb_descriptor_device device_desc = { 0 };

#define min(__a, __b) ((__a) < (__b) ? (__a) : (__b))

static volatile uint32_t *pma_get_io_addr(int ep_no, int field_offset)
{
  volatile char *addr = (volatile char *)USB_RAM;
  addr += reg_read(USB_BTABLE);
  addr += ep_no * 8 * 2;
  addr += field_offset * 2;
  return (volatile uint32_t *)addr;
}

static uint16_t usb_pma_get_tx_addr(int ep_no)
{
  return reg_read(pma_get_io_addr(ep_no, 0)) & 0xffff;
}

static uint16_t usb_pma_get_rx_addr(int ep_no)
{
  return reg_read(pma_get_io_addr(ep_no, 4)) & 0xffff;
}

static uint16_t usb_pma_get_rx_count(int ep_no)
{
  return reg_read(pma_get_io_addr(ep_no, 6)) & u32_bitmask(10);
}

static void usb_prep_tx(int ep_no, const void *src, int numbytes)
{
  bp();
  memcpy_sram_to_pma(usb_pma_get_tx_addr(ep_no), src, numbytes);
  reg_write(pma_get_io_addr(ep_no, 2), numbytes);
  reg32_modify_bits(USB_EP0R, USB_EPXR_STAT_TX,
    USB_EPXR_STAT_TX_WIDTH, USB_EPXR_STAT_TX_VALID);

#if 0
  volatile uint32_t *p = USB_RAM;
  for (int i = 2; i < 256; ++i)
  {
    *p++ = 0x55;
  }
#endif
}

static void usb_handle_get_device_descriptor(int ep_no, int numbytes)
{
  usb_prep_tx(ep_no, &device_desc, min(sizeof(device_desc), numbytes));
}


static void usb_handle_get_descriptor(int ep_no, const struct usb_request *r)
{
  int type = (r->wValue >> 8) & 0xff;
  int index = r->wValue & 0xff;
  if (type == USB_DESCRIPTOR_TYPE_DEVICE) {
    usb_handle_get_device_descriptor(ep_no, r->wLength);
  }
}

static void usb_handle_device_to_host_request(int ep_no, const struct usb_request *r)
{
  if (r->bRequest == USB_SETUP_REQUEST_GET_DESCRIPTOR) {
    usb_handle_get_descriptor(ep_no, r);
  }
}

static void usb_ctr_handler(int ep_no, int dir)
{
  uint32_t ep_info = reg_read(USB_EP0R);
  if (u32_bit_is_set(ep_info, USB_EPXR_CTR_RX)) {
    if (u32_bit_is_set(ep_info, USB_EPXR_SETUP)) {
      struct usb_request r;
      memcpy_pma_to_sram(&r, usb_pma_get_rx_addr(ep_no), usb_pma_get_rx_count(ep_no));
      if (r.bmRequestType == USB_REQUEST_DEVICE_TO_HOST_STANDARD) {
        usb_handle_device_to_host_request(ep_no, &r);
      }
    }
  }
  // uint32_t ep_info = reg_read(0x40006000 + 0x40 * 2);
  usbstats.num_transactions++;
}

void usb_lp_isr(void)
{
  uint32_t v;
  v = reg_read(USB_ISTR);
  if (u32_bit_is_set(v, USB_ISTR_ERR)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_ERR);
    usb_err_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_RESET)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_RESET);
    usb_reset_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_SOF)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_SOF);
    usb_sof_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_ESOF)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_ESOF);
    usb_esof_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_SUSP)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_SUSP);
    usb_susp_handler();
    return;
  }
  if (u32_bit_is_set(v, USB_ISTR_CTR)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_CTR);
    usb_ctr_handler(
      u32_extract_bits(v, USB_ISTR_EP_ID, USB_ISTR_EP_ID_WIDTH),
      u32_extract_bits(v, USB_ISTR_DIR, USB_ISTR_DIR_WIDTH));
  }
  else
  {
    bp();
    while(1);
  }
  reg_write(USB_ISTR, 0);
}

void usb_wakeup_isr(void)
{
  // while(1);
}

static void usb_init_device_descriptor(void)
{
  device_desc.bLength = sizeof(device_desc);
  device_desc.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE;
  device_desc.bcdUSB = 0x0200;
  device_desc.bDeviceClass = 0;
  device_desc.bDeviceSubClass = 0;
  device_desc.bDeviceProtocol = 2;
  device_desc.bMaxPacketSize0 = 8;
  device_desc.idVendor = 0xefef;
  device_desc.idProduct = 0xdcdc;
  device_desc.bcdDevice = 0x102;
  device_desc.iManufacturer = 0;
  device_desc.iProduct = 0;
  device_desc.iSerialNumber = 0;
  device_desc.bNumConfigurations = 1;
}

static void usb_init_descriptors(void)
{
  usb_init_device_descriptor();
}

void sleep_ms(uint32_t)
{
}

uint32_t wait_complete;

void systick_isr(void)
{
  wait_complete = 1;
}

extern void idle(void);

static void systick_wait_ms(uint32_t ms)
{
  reg_write(STK_LOAD, (uint32_t)((float)0x44aa20 / 1000.0f) * ms);

  wait_complete = 0;
  reg_write(STK_CTRL,
    (1<<STK_CTRL_ENABLE) |
    (1<<STK_CTRL_TICKINT));

  idle();
}

void usb_init(void)
{
  usb_init_descriptors();
  rcc_set_72mhz_usb();
  systick_wait_ms(20);
  rcc_enable_usb();
  rcc_enable_gpio_a();
  rcc_enable_afio();
  gpioa_set_cr(11, 3, 2);
  gpioa_set_cr(12, 3, 2);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0);
  reg_write(NVIC_ISER1, 1 << NVIC_INTERRUPT_NUMBER_USB_WAKEUP - 32);
  uint32_t v = reg_read(USB_CNTR);
  reg32_set_bit(USB_CNTR, USB_CNTR_RESETM);
  reg32_clear_bit(USB_CNTR, USB_CNTR_PDWN);
  sleep_ms(1);
  reg32_clear_bit(USB_CNTR, USB_CNTR_FRES);
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
  zero_bss();
  usb_init();

  timer_setup();
//  uart2_setup();
  // adc_setup();
  while(1);
}
