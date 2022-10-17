#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"
#include "usb.h"
#include "rcc.h"
#include "i2c.h"
#include "gpio.h"
#include "ssd1306.h"
#include "scb.h"
#include "font.h"
#include "tim.h"
#include "nvic.h"
#include "string.h"
#include "debug_pin.h"
#include <stdlib.h>

#define min(__a, __b) ((__a) < (__b) ? (__a) : (__b))
#define BRK asm volatile ("bkpt")

#define ARRAY_SIZE(__a) (sizeof(__a) / sizeof(__a[0]))
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

#define USB_NUM_ENDPOINTS 8
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
#define USB_COUNTn_RX_BLSIZE 15
#define USB_COUNTn_RX_BLSIZE_WIDTH 1
#define USB_COUNTn_RX_BLSIZE_2_BYTES 0
#define USB_COUNTn_RX_BLSIZE_32_BYTES 1
#define USB_COUNTn_RX_NUM_BLOCKS 10
#define USB_COUNTn_RX_NUM_BLOCK_WIDTH 5
#define USB_COUNTn_RX_BLSIZE_32_BYTES 1

#define USB_MAX_PACKET_SIZE 64

/* 2 blocks by 32 bytes = 64 bytes */
#define USB_COUNTn_RX_MAX_PACKET_SZ_64 \
  (USB_COUNTn_RX_BLSIZE_32_BYTES << USB_COUNTn_RX_BLSIZE) \
  | ((2 - 1) << USB_COUNTn_RX_NUM_BLOCKS)
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

#define USB_CONFIG_VALUE 1
#define USB_STRING_ID_VENDOR 1
#define USB_STRING_ID_PRODUCT 2
#define USB_STRING_ID_SERIAL 3

const struct usb_descriptor_device device_desc = {
  .bLength = sizeof(struct usb_descriptor_device),
  .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
  .bcdUSB = 0x0200,
  .bDeviceClass = 0xff,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = 64,
  .idVendor = 0xd001,
  .idProduct = 0xd002,
  .bcdDevice = 0x0200,
  .iManufacturer = USB_STRING_ID_VENDOR,
  .iProduct = USB_STRING_ID_PRODUCT,
  .iSerialNumber = USB_STRING_ID_SERIAL,
  .bNumConfigurations = 1
};

const struct usb_descriptor_device_qualifier device_qual = {
  .bLength = sizeof(struct usb_descriptor_device_qualifier),
  .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
  .bcdUSB = 0x0200,
  .bDeviceClass = 0xff,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = 64,
  .bNumConfigurations = 1,
  .bReserved = 0
};

struct usb_config_full {
  struct usb_descriptor_config c;
  struct usb_descriptor_interface i;
  struct usb_descriptor_endpoint ep;
};

const struct usb_config_full config_desc = {
  .c = {
    .bLength = sizeof(struct usb_descriptor_config),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
    .wTotalLength = sizeof(struct usb_config_full),
    .bNumInterfaces = 1,
    .bConfigurationValue = USB_CONFIG_VALUE,
    .iConfiguration = 0,
    .bmAttributes = 0x80, /* bit 7 is reserved as 1 */
    .bMaxPower = 50 /* max power = 100mA */
  },
  .i = {
    .bLength = sizeof(struct usb_descriptor_interface),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternativeSetting = 0,
    .bNumEndpoints = 1,
    .bInterfaceClass = 2, /* CDC */
    .bInterfaceSubClass = 2, /* CDC ACM */
    .bInterfaceProtocol = 1, /* Common AT commands */
    .iInterface = 0
  },
  .ep = {
    .bLength = sizeof(struct usb_descriptor_endpoint),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = 1,
    .bmAttributes = USB_EP_ATTRIBUTE_TYPE_BULK,
    .wMaxPacketSize = 256,
    .bInterval = 0
  }
};

const struct usb_descriptor_string_0 string_desc_0 = {
  .bLength = sizeof(struct usb_descriptor_string_0),
  .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  .wLangId = 0x0409
};

struct usb_descriptor_string_vendor {
  struct usb_descriptor_string_header h;
  char buf[14 * 2];
};

const struct usb_descriptor_string_vendor vendor_string_desc = {
  .h = {
    .bLength = sizeof(struct usb_descriptor_string_vendor),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  },
  .buf = {
    'C', 0,
    '4', 0,
    'a', 0,
    '0', 0,
    '$', 0,
    '-', 0,
    'E', 0,
    'n', 0,
    '9', 0,
    '1', 0,
    'N', 0,
    'E', 0,
    'E', 0,
    'r', 0,
  }
};

struct usb_descriptor_string_product {
  struct usb_descriptor_string_header h;
  char buf[13 * 2];
};

const struct usb_descriptor_string_product product_string_desc = {
  .h = {
    .bLength = sizeof(struct usb_descriptor_string_product),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  },
  .buf = {
    'r', 0,
    'o', 0,
    't', 0,
    't', 0,
    'e', 0,
    'd', 0,
    '.', 0,
    'O', 0,
    's', 0,
    'C', 0,
    '1', 0,
    'l', 0,
    'l', 0,
  }
};

struct usb_descriptor_string_serial {
  struct usb_descriptor_string_header h;
  char buf[11 * 2];
};

const struct usb_descriptor_string_serial serial_string_desc = {
  .h = {
    .bLength = sizeof(struct usb_descriptor_string_serial),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
  },
  .buf = {
    '0', 0,
    'x', 0,
    'd', 0,
    '0', 0,
    '0', 0,
    '1', 0,
    '0', 0,
    'x', 0,
    'd', 0,
    '0', 0,
    '0', 0,
  }
};



static void pmacpy_from(void *dst, uint32_t pma_offset, int num_bytes)
{
  const volatile uint32_t *from = PMA_TO_SRAM_ADDR(pma_offset);
  /* We are reading 4 bytes, extract only lower 2-byte part of it */
  int i;
  for (i = 0; i < num_bytes / 2; i++) {
    ((uint16_t *)dst)[i] = from[i] & 0xffff;
  }
}

static void pmacpy_to(uint32_t pma_dest, const void *src, uint16_t num_bytes)
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

struct usb_interrupt_stats {
  int num_sofs;
  int num_esofs;
  int num_errs;
  int num_susp;
  int num_wkup;
  int num_resets;
    /* 0:GET_DESCRIPTOR */
    /* 1:GET_DESCRIPTOR_IN_COMPLETED */
    /* 2:RESET */
    /* 3:SET_ADDRESS */
    /* 4:SET_ADDRESS_IN */
    /* 5:GET_DESCRIPTOR */
  int num_transactions;
};

static struct usb_interrupt_stats usbstats = { 0 };


static void update_display_usb_stats()
{
  char buf[32];
  dbuf_draw_text(20, 4, "r:", &font_1);
  itoa(usbstats.num_resets, buf, 10);
  dbuf_draw_text(28, 4, buf, &font_1);
  itoa(usbstats.num_errs, buf, 10);
  dbuf_draw_text(34, 4, buf, &font_1);
  itoa(usbstats.num_transactions, buf, 10);
  dbuf_draw_text(40, 4, buf, &font_1);
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

/*
 * Initialize buffer description table
 * 0x40006000 (00) ADDR0_TX
 * 0x40006004 (02) COUNT0_TX
 * 0x40006008 (04) ADDR0_RX
 * 0x4000600c (06) COUNT0_RX

 * 0x40006010 (08) ADDR1_TX
 * 0x40006014 (0a) COUNT1_TX
 * 0x40006018 (0c) ADDR1_RX
 * 0x4000601c (0e) COUNT1_RX

 * 0x40006020 (10) ADDR2_TX
 * 0x40006024 (12) COUNT2_TX
 * 0x40006028 (14) ADDR2_RX
 * 0x4000602c (16) COUNT2_RX
 * ......
 * 0x40006020 (38) ADDR7_TX
 * 0x40006024 (3a) COUNT7_TX
 * 0x40006028 (3c) ADDR7_RX
 * 0x4000602c (40) COUNT7_RX
 * Table describes 8 endpoints.
 * Each endpoint has 8 bytes of data:
 * tx addr(2bytes), tx count(2bytes), rx addr(2bytes), rx count(2 bytes)
 * so Buffer descriptor table size is 8 * 8 = 64
 * Start of free area is after these first 64 bytes (0x40)
 */
static void pma_init_bdt(void)
{
  uint16_t ep_buf_addr;
  struct ep_buf_desc ebdt[USB_NUM_ENDPOINTS];

  ep_buf_addr = sizeof(ebdt);

  for (int i = 0; i < ARRAY_SIZE(ebdt); ++i) {
    ebdt[i].tx_addr = sizeof(ebdt) + 64 * (i + 0);
    ebdt[i].tx_count = 0;
    ebdt[i].rx_addr = sizeof(ebdt) + 64 * (i + 1);
    ebdt[i].rx_count = USB_COUNTn_RX_MAX_PACKET_SZ_64;
  }
  pmacpy_to(reg_read(USB_BTABLE), ebdt, sizeof(ebdt));
}

/*
 * During store op to EPxR register bits CTR_TX and CTR_RX
 * when set to 1 are ignored and maintain their value.
 * Not initializing these bits to 1 will initialize them to 0
 * Writing 0 is same as clearing (and thus missing) one of those
 * two events
 */
#define USB_EPxR_STATIC_BITS(__ep) \
  ((USB_EPXR_EP_TYPE_CONTROL << USB_EPXR_EP_TYPE) \
    | (1 << USB_EPXR_EP_KIND) \
    | (1 << USB_EPXR_EP_TYPE) \
    | (1 << USB_EPXR_CTR_TX) \
    | (1 << USB_EPXR_CTR_RX) \
    | (__ep & 0xf))

static void usb_ep_init(void)
{
  static int xx = 0;
  uint32_t v = USB_EPxR_STATIC_BITS(0);
  v |= USB_EPXR_STAT_TX_NAK << USB_EPXR_STAT_TX;
  v |= USB_EPXR_STAT_RX_VALID << USB_EPXR_STAT_RX;
  reg_write(USB_EP0R, v);
}

static void usb_reset_handler(void)
{
  uint32_t v;
  usbstats.num_resets++;
  if (reg_read(USB_DADDR) & 0x7f)
    BRK;

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
  // reg32_set_bit(USB_CNTR, USB_CNTR_FSUSP);
  usbstats.num_susp++;
}

static void usb_wakeup_handler(void)
{
  usbstats.num_wkup++;
  // reg32_set_bit(USB_CNTR, USB_CNTR_RESUME);
}

static void bp(void)
{
}

static void usb_err_handler(void)
{
  if (usbstats.num_errs)
    BRK;
  usbstats.num_errs++;
}

#define min(__a, __b) ((__a) < (__b) ? (__a) : (__b))

#define BTABLE_EP_FIELD_OFFSET(__ep, __field_off) (__ep * 8 + __field_off * 2)
#define BTABLE_FIELD_OFFSET_TX_ADDR(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 0)

#define BTABLE_FIELD_OFFSET_TX_COUNT(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 2)

#define BTABLE_FIELD_OFFSET_RX_ADDR(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 4)

#define BTABLE_FIELD_OFFSET_RX_COUNT(__ep) \
  BTABLE_EP_FIELD_OFFSET(__ep, 6)

static volatile uint32_t *pma_get_io_addr(int ep_no, int field_offset)
{
  volatile char *addr = ((volatile char *)USB_RAM) + reg_read(USB_BTABLE);
  return (volatile uint32_t *)(addr
    + BTABLE_EP_FIELD_OFFSET(ep_no, field_offset));
}

static uint16_t usb_pma_get_tx_addr(int ep_no)
{
  return reg_read(pma_get_io_addr(ep_no, 0)) & 0xffff;
}

static void usb_pma_set_tx_count(int ep_no, uint16_t value)
{
  reg_write(pma_get_io_addr(ep_no, 2), value);
}

static uint16_t usb_pma_get_tx_count(int ep_no)
{
  return reg_read(pma_get_io_addr(ep_no, 2)) & 0xffff;
}

static uint16_t usb_pma_get_rx_addr(int ep_no)
{
  return reg_read(pma_get_io_addr(ep_no, 4)) & 0xffff;
}

static uint16_t usb_pma_get_rx_count(int ep_no)
{
  return reg_read(pma_get_io_addr(ep_no, 6)) & u32_bitmask(10);
}

static uint16_t usb_pma_reset_rx_count(int ep_no)
{
  uint32_t v = reg_read(pma_get_io_addr(ep_no, 6)) & ~u32_bitmask(10);
  return reg_write(pma_get_io_addr(ep_no, 6), v);
}

struct usb_ctr_handler_log_entry {
  uint32_t dir;
  uint32_t is_setup;
  uint32_t epxr_on_entry;
  uint32_t epxr_on_exit;
};

static struct usb_ctr_handler_log_entry log[32] = { 0 };
static int log_idx = 0;

static uint32_t last_ep0r;

uint16_t addr = 0;
static void usb_ep_tx_nak_to_valid(int ep)
{
  uint32_t v = USB_EPxR_STATIC_BITS(ep);
  reg_write(USB_EP0R, v | ((2 ^ 3)<<4)|addr);
}

static void usb_ep_rx_nak_to_valid(int ep)
{
  uint32_t v = USB_EPxR_STATIC_BITS(ep);
  reg_write(USB_EP0R, v | ((2 ^ 3)<<12) | addr);
}

static void usb_ep_tx_nak_to_stall(int ep)
{
  uint32_t v = USB_EPxR_STATIC_BITS(ep);
  reg_write(USB_EP0R, v | ((2 ^ 1)<<4));
}

static void usb_ep_set_status_out(int ep)
{
  uint32_t v = USB_EPxR_STATIC_BITS(ep);
  reg_write(USB_EP0R, v | (1<<8));
}

static void usb_ep_rx_nak_to_stall(int ep)
{
  uint32_t v = USB_EPxR_STATIC_BITS(ep);
  reg_write(USB_EP0R, v | ((2 ^ 1)<<12));
}

static void usb_handle_set_address(uint16_t address)
{
  uint32_t v = 0;
  u32_modify_bits(&v, USB_DADDR_ADDR, USB_DADDR_ADDR_WIDTH, address);
  u32_set_bit(&v, USB_DADDR_EF);
  reg_write(USB_DADDR, v);
}

static void usb_handle_get_descriptor(int ep, const struct usb_request *r)
{
  int type = (r->wValue >> 8) & 0xff;
  int index = r->wValue & 0xff;
  /*
   * num_transaction = 0 GET_DESCRIPTOR / DEVICE
   * num_transaction = 5 GET_DESCRIPTOR / DEVICE
   */
  if (type == USB_DESCRIPTOR_TYPE_DEVICE) {
    uint16_t tx_count = min(r->wLength, sizeof(device_desc));
    pmacpy_to(usb_pma_get_tx_addr(ep), &device_desc, tx_count);
    usb_pma_set_tx_count(ep, tx_count);
    usb_ep_tx_nak_to_valid(ep);
  } else if (type == USB_DESCRIPTOR_TYPE_CONFIGURATION) {
    uint16_t tx_count = min(r->wLength, sizeof(config_desc));
    pmacpy_to(usb_pma_get_tx_addr(ep), &config_desc, tx_count);
    usb_pma_set_tx_count(ep, tx_count);
    usb_ep_tx_nak_to_valid(ep);
  } else if (type == USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER) {
    uint16_t tx_count = min(r->wLength, sizeof(device_qual));
    pmacpy_to(usb_pma_get_tx_addr(ep), &device_qual, tx_count);
    usb_pma_set_tx_count(ep, tx_count);
    usb_ep_tx_nak_to_valid(ep);
  } else if (type == USB_DESCRIPTOR_TYPE_STRING) {
    if (index == 0) {
      uint16_t tx_count = min(r->wLength, sizeof(string_desc_0));
      pmacpy_to(usb_pma_get_tx_addr(ep), &string_desc_0, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    } else if (index == USB_STRING_ID_VENDOR) {
      uint16_t tx_count = min(r->wLength, sizeof(vendor_string_desc));
      pmacpy_to(usb_pma_get_tx_addr(ep), &vendor_string_desc, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    } else if (index == USB_STRING_ID_PRODUCT) {
      uint16_t tx_count = min(r->wLength, sizeof(product_string_desc));
      pmacpy_to(usb_pma_get_tx_addr(ep), &product_string_desc, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    } else if (index == USB_STRING_ID_SERIAL) {
      uint16_t tx_count = min(r->wLength, sizeof(serial_string_desc));
      pmacpy_to(usb_pma_get_tx_addr(ep), &serial_string_desc, tx_count);
      usb_pma_set_tx_count(ep, tx_count);
      usb_ep_tx_nak_to_valid(ep);
    }
  } else {
    BRK;
  }
}

static int next_addr = 0;
static int next_config = 0;
static bool usb_config_initialized = 0;

static void usb_handle_host_to_device_request(int ep,
  const struct usb_request *r)
{
  if (r->bRequest == USB_SETUP_REQUEST_SET_ADDRESS) {
    usb_pma_set_tx_count(ep, 0);
    usb_ep_tx_nak_to_valid(ep);
    next_addr = r->wValue & 0xf;
  } else if (r->bRequest == USB_SETUP_REQUEST_SET_CONFIGURATION) {
    usb_pma_set_tx_count(ep, 0);
    usb_ep_tx_nak_to_valid(ep);
    next_config = r->wValue & 0xf;
  }
  else {
    BRK;
  }
}

static void usb_handle_device_to_host_request(int ep,
  const struct usb_request *r)
{
  if (r->bRequest == USB_SETUP_REQUEST_GET_DESCRIPTOR) {
    usb_handle_get_descriptor(ep, r);
  }
  else {
    BRK;
  }
}

static void usb_ep_clear_ctr_tx(int ep)
{
  uint32_t v = USB_EPxR_STATIC_BITS(ep);
  u32_clear_bit(&v, USB_EPXR_CTR_TX);
  reg_write(USB_EP0R, v);
}

static void usb_ep_clear_ctr_rx(int ep)
{
  uint32_t v = USB_EPxR_STATIC_BITS(ep);
  u32_clear_bit(&v, USB_EPXR_CTR_RX);
  reg_write(USB_EP0R, v);
}

static int usb_ep_read_rx_packet(int ep, char *buf, int maxsz)
{
  int num_bytes = usb_pma_get_rx_count(ep);
  if (num_bytes <= maxsz) {
    pmacpy_from(buf, usb_pma_get_rx_addr(ep), num_bytes);
  }

  return num_bytes;
}

static void usb_ctr_handler(int ep, int dir)
{
  struct usb_ctr_handler_log_entry *l = &log[log_idx++];
  l->epxr_on_entry = reg_read(USB_EP0R);
  l->dir = dir;
  l->is_setup = u32_bit_is_set(l->epxr_on_entry, USB_EPXR_SETUP);

  if (dir) {
    /* CTR_RX should be set because this is an RX (OUT or SETUP) packet */
    if (!reg32_bit_is_set(USB_EP0R, USB_EPXR_CTR_RX)) {
    }
    usb_ep_clear_ctr_rx(ep);
    if (reg32_bit_is_set(USB_EP0R, USB_EPXR_SETUP)) {
      struct usb_request r;
      if (usb_ep_read_rx_packet(ep, (void *)&r, sizeof(r)) < sizeof(r)) {
      }
      usb_pma_reset_rx_count(ep);

      if (r.bmRequestType == USB_REQUEST_DEVICE_TO_HOST_STANDARD) {
        usb_handle_device_to_host_request(ep, &r);
      }
      else if (r.bmRequestType == USB_REQUEST_HOST_TO_DEVICE_STANDARD) {
        usb_handle_host_to_device_request(ep, &r);
      }
      else {
        BRK;
      }
    }
  } else {
    if (next_addr) {
      usb_handle_set_address(next_addr);
      next_addr = 0;
    }
    if (next_config) {
      if (next_config == USB_CONFIG_VALUE) {
        usb_config_initialized = 1;
      }
      next_config = 0;
    }

    usb_ep_clear_ctr_tx(ep);
    usb_pma_set_tx_count(ep, 0);
    usb_ep_tx_nak_to_stall(ep);
    usb_ep_rx_nak_to_valid(ep);
  }
  usbstats.num_transactions++;
  l->epxr_on_exit= reg_read(USB_EP0R);
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
  if (u32_bit_is_set(v, USB_ISTR_WKUP)) {
    reg32_clear_bit(USB_ISTR, USB_ISTR_WKUP);
    usb_wakeup_handler();
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
    reg_write(USB_ISTR, 0);
    bp();
    while(1);
  }
}

void usb_wakeup_isr(void)
{
  // while(1);
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
  systick_wait_ms(20);
  rcc_enable_usb();
  rcc_enable_gpio_a();
  rcc_enable_afio();
  // TODO As soon as the USB is enabled, these pins are automatically connected to
  // the USB internal transceiver.
#if 0
  gpioa_set_cr(11, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
  gpioa_set_cr(12, GPIO_MODE_OUT_50_MHZ, GPIO_CNF_OUT_ALT_PUSH_PULL);
#endif
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_HP_CAN_TX);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0);
  reg_write(NVIC_ISER1, 1 << NVIC_INTERRUPT_NUMBER_USB_WAKEUP - 32);

  /* Enable interrupts on RESET request from host */
  reg32_set_bit(USB_CNTR, USB_CNTR_RESETM);
  /* Enable interrupts on ERR condition */
  reg32_set_bit(USB_CNTR, USB_CNTR_ERRM);

  /* Disable power-down bit, USB peripheral powers up */
  reg32_clear_bit(USB_CNTR, USB_CNTR_PDWN);

  sleep_ms(1);
  /* On power clear Force-RESET bit to exit reset state */
  reg32_clear_bit(USB_CNTR, USB_CNTR_FRES);
  /* Clear all possible spurious interrupts */
  reg_write(USB_ISTR, 0);
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
  struct scb_cpuid i;
  scb_get_cpuid(&i);
  // scb_set_prigroup(3);
  nvic_set_priority(NVIC_INTERRUPT_NUMBER_ADC1, 1);
  nvic_set_priority(NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0, 0);
  /*
   * usb irq should have higher priority than ADC or else it never gets
   * a chance to execute
   */
  int usb_irq_pri =  nvic_get_priority(NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0);
  int adc_pri =  nvic_get_priority(NVIC_INTERRUPT_NUMBER_ADC1);

  zero_bss();
  rcc_set_72mhz_usb();
#if 0
  i2c_init();
  ssd1306_init();
  timer_setup();
  debug_pin_setup();
  update_display();
#endif
  usb_init();
//  uart2_setup();

//  adc_setup();

  while(1);
}
