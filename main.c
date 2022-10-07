#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"
#include "usb.h"
#include "rcc.h"
#include "i2c.h"
#include "gpio.h"
#include "ssd1306.h"
#include "font.h"
#include "tim.h"
#include "nvic.h"
#include "string.h"
#include "debug_pin.h"
#include <stdlib.h>


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

#if 0
uint16_t tim_calc_psc(float timeout, uint32_t f_clk, uint16_t auto_reload_value)
{
  return ;
}
#endif

static volatile int last_adc = 0;

int x = 0;

int dig = 0;
char buf[32];
int tick = 0;

struct bar_widget {
  int pos_x;
  int pos_y;
  int size_x;
  int size_y;
  int level;
};

void bar_widget_draw(const struct bar_widget *b)
{
  dbuf_draw_rect(
    b->pos_x, b->pos_y,
    b->pos_x + b->size_x,
    b->pos_y + b->size_y, 1);

  dbuf_draw_hatched_rect(
    b->pos_x + 2,
    b->pos_y,
    b->pos_x + b->size_x - 1,
    b->pos_y + b->level,
    1);

  dbuf_draw_line(
    b->pos_x + 2,
    b->pos_y + b->level,
    b->pos_x + b->size_x - 1,
    b->pos_y + b->level,
    1);
}

void draw_tim2_cntr(int x, int y)
{
  char counterbuf[32];
  itoa(tim2_read_counter_value(), counterbuf, 10);
  x = dbuf_draw_text(x, y, "TIM2_TCNT:", &font_1);
  dbuf_draw_text(x, y, counterbuf, &font_2);
}

void draw_dynamic_bar(int level)
{
  struct bar_widget w = {
    .pos_x = 110,
    .pos_y = 0,
    .size_x = 10,
    .size_y = 63,
    .level = level
  };
  bar_widget_draw(&w);
}

void draw_voltmeter(int x, int y, int volt_int, int volt_frac)
{
  char b[32];
  char *p = b;
  itoa(volt_int, p, 10);
  p += strlen(p);
  *p++ = '.';
  itoa(volt_frac, p, 10);
  p += strlen(p);
  x = dbuf_draw_text(x, y, "V:", &font_3);
  dbuf_draw_text(x, y, b, &font_3);
}

int frame_counter = 0;
int duration;

void draw_blinker_icon(int x, int y, int sz, int num_frames, int interval)
{
  if (!duration && !(frame_counter % interval)) {
    duration = num_frames;
  }
  if (duration) {
    dbuf_draw_filled_rect(x, y, x + sz, y + sz, 1);
    duration--;
  }
}

static void update_display_usb_stats()
{
  dbuf_draw_text(20, 4, "sof:", &font_1);
}

void update_display()
{
  frame_counter++;
  int y;
  float adc_normalized = (float)last_adc / 4096;
  int level = adc_normalized * 64;
  int num_volt = adc_normalized * 3300;
  int first = num_volt / 1000;
  int next = num_volt - first * 1000;


  dbuf_clear();
  update_display_usb_stats();
  draw_dynamic_bar(level);
  draw_voltmeter(2, 35, first, next);

  y = 40;
  dbuf_draw_line(64, y, 64, y + 3, 1);
  y = 20;
  dbuf_draw_line(64, y, 64, y + 3, 1);
  y = 5;
  dbuf_draw_line(64, y, 64, y + 3, 1);
  draw_tim2_cntr(10, 50);
  draw_blinker_icon(89, 6, 5, 3, 13);
  dbuf_flush();
}

void adc_isr(void)
{
  last_adc = reg_read(ADC1_DR);
  update_display();
  reg_write(ADC1_SR, 0);
  reg_write(ADC1_CR2, 1 << ADC_CR2_EON);
}

void adc_setup(void)
{
  rcc_enable_gpio_a();
  rcc_enable_adc1();
  gpioa_set_cr(1, GPIO_MODE_INPUT, GPIO_CNF_IN_ANALOG);

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
  rcc_enable_gpio_a();
  rcc_enable_usart2();

  gpioa_set_cr(2, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
  gpioa_set_cr(3, GPIO_MODE_INPUT     , GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpioa_set_odr(3);
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
  tim2_setup(true, CALC_PSC(0.1, F_CLK, 0xffff), 0xffff, true, true);
}

void tim2_isr_cb()
{
  debug_pin_toggle();
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
  systick_wait_ms(20);
  rcc_enable_usb();
  rcc_enable_gpio_a();
  rcc_enable_afio();
  // TODO As soon as the USB is enabled, these pins are automatically connected to
  // the USB internal transceiver.
  gpioa_set_cr(11, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
  gpioa_set_cr(12, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
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

static void i2c_init(void)
{
  rcc_enable_i2c1();
  rcc_enable_gpio_b();
  /* B6 - SDA Alternate function open drain */
  gpiob_set_cr(6, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_OPEN_DRAIN);
  /* B7 - SCL Alternate function open drain */
  gpiob_set_cr(7, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_OPEN_DRAIN);
  gpio_remap_i2c1(GPIO_REMAP_I2C1_PB6_PB7);
  i2c_clock_setup();
}

void main(void)
{
  zero_bss();
  rcc_set_72mhz_usb();
  i2c_init();
  ssd1306_init();
  timer_setup();
  debug_pin_setup();

  usb_init();
//  uart2_setup();

  adc_setup();
  while(1);
}
