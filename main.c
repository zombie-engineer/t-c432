#include "rcc.h"
#include "ui.h"
#include "debug_pin.h"
#include "adc.h"
#include "scheduler.h"
#include "task.h"
#include "pushbuttons.h"
#include "compiler.h"
#include "debug_pin.h"
#include <svc.h>
#include "display_hw.h"
#include "main_task.h"
#include "common_util.h"
#include "dma.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* __errno symbol must be defined to link with log function in libm.a */
void __errno(void) {}

void tim2_isr_cb() {}

extern uint32_t __bss_start;
extern uint32_t __bss_end;

static void zero_bss(void)
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
    asm volatile ("wfi");
  }
}

static void setup_adc(void)
{
  rcc_periph_ena(RCC_PERIPH_ADC1);
  adc_init();

#define ADC_CH_INIT(__ch_idx, __sampling_time) \
  { .idx = __ch_idx, \
    .sampling_time = __sampling_time \
  }
  struct adc_scan_mode_channel_config regular_group[] = {
    ADC_CH_INIT(ADC_CH_TEMP_SENSOR0, ADC_SAMPTIME_1_5_CYCLES),
    ADC_CH_INIT(ADC_CH_TEMP_SENSOR1, ADC_SAMPTIME_1_5_CYCLES),
    ADC_CH_INIT(ADC_CH_TEMP_SENSOR2, ADC_SAMPTIME_239_5_CYCLES),
    ADC_CH_INIT(ADC_CH_VREF, ADC_SAMPTIME_1_5_CYCLES),
  };

  struct adc_scan_mode_config cfg = {
    .regular_group = regular_group,
    .regular_group_size = ARRAY_SIZE(regular_group),
    .injected_group = NULL,
    .injected_group_size = 0
  };

  adc_init_scan_mode(0, &cfg);
}

static void setup_gpio(void)
{
  rcc_periph_ena(RCC_PERIPH_IOPA);
  rcc_periph_ena(RCC_PERIPH_IOPB);
  rcc_periph_ena(RCC_PERIPH_IOPC);
  rcc_periph_ena(RCC_PERIPH_SPI1);
  /* GPIO port B and AFIO needed to be enabled for push buttons */
  rcc_periph_ena(RCC_PERIPH_AFIO);

  /* Thermostat */
  gpio_setup(TEMP_SENSOR0_GPIO_PORT, TEMP_SENSOR0_GPIO_PIN, GPIO_MODE_INPUT,
    GPIO_CNF_IN_ANALOG);

  gpio_setup(TEMP_SENSOR1_GPIO_PORT, TEMP_SENSOR1_GPIO_PIN, GPIO_MODE_INPUT,
    GPIO_CNF_IN_ANALOG);

  gpio_setup(THERMOSTAT_ENABLE_GPIO_PORT, THERMOSTAT_ENABLE_GPIO_PIN,
    GPIO_MODE_OUT_10_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);

  /* Pump */
  gpio_setup(PUMP_ENABLE_GPIO_PORT, PUMP_ENABLE_GPIO_PIN, GPIO_MODE_OUT_10_MHZ,
    GPIO_CNF_OUT_GP_PUSH_PULL);

  gpio_setup(FLOW_METER_GPIO_PORT, FLOW_METER_GPIO_PIN,
    GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);

  gpio_setup(GPIO_PORT_B, 10, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpio_setup(GPIO_PORT_B, 11, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpio_setup(GPIO_PORT_B, 13, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpio_setup(GPIO_PORT_B, 14, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpio_setup(GPIO_PORT_B, 15, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
}

static void setup_usb(void)
{
#if ENABLE_ADC == 1 && ENABLE_USB == 1
  nvic_set_priority(NVIC_INTERRUPT_NUMBER_ADC1, 1);
  nvic_set_priority(NVIC_INTERRUPT_NUMBER_USB_LP_CAN_RX0, 0);
  /*
   * usb irq should have higher priority than ADC or else it never gets
   * a chance to execute
   */
#endif
}

static void setup_main_clock(void)
{
  rcc_set_72mhz_usb();
}

static void setup_dma(void)
{
  dma_init();
}

void setup_peripherals(void)
{
  setup_main_clock();
  setup_dma();
  setup_gpio();
  setup_adc();
  debug_pin_setup();
}

static void setup_rtos(void)
{
  struct task *t;
  struct task *main_task;

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
}

void main(void)
{
  zero_bss();

  setup_peripherals();
  setup_rtos();
}
