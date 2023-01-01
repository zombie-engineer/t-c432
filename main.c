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
#include "pushbuttons.h"
#include "compiler.h"
#include "dma.h"
#include "stm32f103_pin_config.h"

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
  rcc_periph_ena(RCC_PERIPH_I2C1);
  /* B6 - SDA Alternate function open drain */
  gpio_setup(I2C1_SDA_PORT, I2C1_SDA_PIN_6, GPIO_MODE_OUT_50_MHZ,
    GPIO_CNF_OUT_ALT_OPEN_DRAIN);
  /* B7 - SCL Alternate function open drain */
  gpio_setup(I2C1_SCL_PORT, I2C1_SCL_PIN_7, GPIO_MODE_OUT_50_MHZ,
    GPIO_CNF_OUT_ALT_OPEN_DRAIN);
  gpio_remap_i2c1(GPIO_REMAP_I2C1_PB6_PB7);
  i2c_clock_setup();
  i2c_init_isr(true);
}

void test_task(void *arg)
{
  pushbuttons_init();
  i2c_init();
  ssd1306_init();

  while(1) {
    ui_update();
    debug_pin_toggle();
    asm volatile ("wfi");
  }
}

void main(void)
{
  struct scb_cpuid i;
  struct task *t;

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
  t = task_create("main", main_task, scheduler_exit_task);
  if (!t) {
    BRK;
    while(1);
  }

  /* GPIO port B and AFIO needed to be enabled for push buttons */
  rcc_periph_ena(RCC_PERIPH_AFIO);
  rcc_periph_ena(RCC_PERIPH_IOPB);

  debug_pin_setup();

  scheduler_init();
  scheduler_enqueue_runnable(t);

  t = task_create("test_task", test_task, scheduler_exit_task);
  if (!t) {
    BRK;
    while(1);
  }

  scheduler_enqueue_runnable(t);
  scheduler_start();

#if 0
  usb_init();
//  uart2_setup();
//  adc_setup();
  while(1);
#endif
}
