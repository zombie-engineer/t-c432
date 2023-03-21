#include <stdint.h>
#include <stdbool.h>
#include "reg_access.h"
#include "rcc.h"
#include "i2c.h"
#include "gpio.h"
#include "ui.h"
#include "scb.h"
#include "tim.h"
#include "usart.h"
#include "nvic.h"
#include "string.h"
#include "debug_pin.h"
#include "adc.h"
#include "scheduler.h"
#include "task.h"
#include "main_task.h"
#include "pushbuttons.h"
#include "compiler.h"
#include "dma.h"
#include "stm32f103_pin_config.h"
#include "debug_pin.h"
#include <svc.h>
#include <stdlib.h>
#include "display_hw.h"


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

void ui_task(void *arg)
{
  display_hw_init();
  ui_init();
  pushbuttons_init();

  while(1) {
    svc_wait_ms(20);
    ui_tick(20);
    ui_redraw();
    debug_pin_toggle();
    asm volatile ("wfi");
  }
}

void main(void)
{
  struct scb_cpuid i;
  struct task *t;
  struct task *main_task;

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
  rcc_periph_ena(RCC_PERIPH_IOPA);
  rcc_periph_ena(RCC_PERIPH_IOPB);
  rcc_periph_ena(RCC_PERIPH_IOPC);
  rcc_periph_ena(RCC_PERIPH_SPI1);
  /* GPIO port B and AFIO needed to be enabled for push buttons */
  rcc_periph_ena(RCC_PERIPH_AFIO);

  debug_pin_setup();

  scheduler_init();
  main_task = task_create("main", main_task_fn, scheduler_exit_task);
  if (!main_task) {
    BRK;
    while(1);
  }

  t = task_create("ui_task", ui_task, scheduler_exit_task);
  if (!t) {
    BRK;
    while(1);
  }

  scheduler_enqueue_runnable(t);
  scheduler_start(main_task);

#if 0
//  uart2_setup();
//  adc_setup();
  while(1);
#endif
}
