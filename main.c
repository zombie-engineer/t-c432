#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"
#include "rcc.h"
#include "i2c.h"
#include "gpio.h"
#include "ssd1306.h"
#include "ui.h"
#include "scb.h"
#include "tim.h"
#include "usart.h"
#include "nvic.h"
#include "string.h"
#include "debug_pin.h"
#include "adc.h"
#include "usb_driver.h"
#include <stdlib.h>
#include "scheduler.h"
#include "task.h"
#include "main_task.h"
#include "compiler.h"

#define F_CLK 36000000

#if 0
uint16_t tim_calc_psc(float timeout, uint32_t f_clk, uint16_t auto_reload_value)
{
  return ;
}
#endif


int x = 0;

int dig = 0;
char buf[32];
int tick = 0;

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
  BRK;
  task_create("main", main_task);
  scheduler_start();
  i2c_init();
  ssd1306_init();
  timer_setup();
  debug_pin_setup();
  ui_update();
  usb_init();
//  uart2_setup();

//  adc_setup();

  while(1);
}
