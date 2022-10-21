#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"
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
#include "usb_driver.h"
#include <stdlib.h>



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

#define THUMB __attribute__((target("thumb")))

#define F_CLK 36000000

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

#if 0
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
#endif

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
  // update_display_usb_stats();
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

  zero_bss();
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

  rcc_set_72mhz_usb();
  i2c_init();
  ssd1306_init();
  timer_setup();
  debug_pin_setup();
  update_display();
  usb_init();
//  uart2_setup();

//  adc_setup();

  while(1);
}
