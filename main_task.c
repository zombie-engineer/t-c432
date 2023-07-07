#include "main_task.h"
#include "systick.h"
#include <ui.h>
#include "adc.h"
#include <tim.h>
#include <rcc.h>
#include <usb_driver.h>
#include <gpio.h>
#include <svc.h>
#include <config.h>
#include "drivers/ws2812b/ws2812b.h"
#include "debug_pin.h"

void timer_setup(void)
{
  /* SYSCLK = 72MHz */
  rcc_periph_ena(RCC_PERIPH_TIM2);
  tim2_setup(true, CALC_PSC(0.1, F_CPU, 0xffff), 0xffff, true, true);
}

void usb_set_address_callback(void *arg)
{
  ui_on_set_address();
}

void usb_tx_callback(void *arg)
{
  ui_on_tx(arg);
}

void usb_rx_callback(void *arg)
{
  ui_on_rx(arg);
}

extern uint16_t adc_buf[128];

#define TERMO_STATE_COOLING 0
#define TERMO_STATE_HEATING_ACTIVE 1
#define TERMO_STATE_HEATING_INERTIAL 2

int termo_state = TERMO_STATE_COOLING;

int termo_force_heating_timer = 0;
void termo_heater_enable(void)
{
  debug_pin_off();
  gpio_odr_modify(GPIO_PORT_B, 5, 1);
}

void termo_heater_disable(void)
{
  debug_pin_on();
  gpio_odr_modify(GPIO_PORT_B, 5, 0);
}

void manage_termo()
{
  uint16_t v = adc_buf[0];
  uint32_t sum_v = 0;
  for (int i = 0; i < 128; ++i)
    sum_v += adc_buf[i];

  sum_v /= 128;

  if (termo_state == TERMO_STATE_COOLING) {
    if (sum_v < 2500) {
      termo_state = TERMO_STATE_HEATING_ACTIVE;
      termo_heater_enable();
      termo_force_heating_timer = 100;
    }
  } else if (termo_state == TERMO_STATE_HEATING_ACTIVE) {
    termo_force_heating_timer--;
    if (termo_force_heating_timer <= 0) {
      /* heating is continued */
      termo_heater_disable();
      termo_state = TERMO_STATE_HEATING_INERTIAL;
      termo_force_heating_timer = 1200;
    }
  } else if (termo_state == TERMO_STATE_HEATING_INERTIAL) {
    termo_force_heating_timer--;
    if (termo_force_heating_timer <= 0) {
      termo_state = TERMO_STATE_COOLING;
    }
  }
}

void main_task_fn(void *)
{
  gpio_setup(GPIO_PORT_B, 5, GPIO_MODE_OUT_10_MHZ, GPIO_CNF_OUT_GP_PUSH_PULL);
  gpio_odr_modify(GPIO_PORT_B, 5, 1);
  systick_set(CNF_SCHEDULER_TICK_MS);
  timer_setup();
#if 0
  usb_driver_set_cb(USB_CB_SET_ADDRESS, usb_set_address_callback);
  usb_driver_set_cb(USB_CB_RX, usb_rx_callback);
  usb_driver_set_cb(USB_CB_TX, usb_tx_callback);
  usb_init();
#endif
  adc_setup();
  // ws2812b_init();
  while(1) {
    manage_termo();
    // ws2812b_update();
    svc_wait_ms(10);
    asm volatile ("wfe");
  }
}
