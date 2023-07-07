#pragma once
#include "config.h"

#define DEBUG_PIN_DISABLED 0
#define DEBUG_PIN_ON 1
#define DEBUG_PIN_OFF 2

static int debug_pin_status = DEBUG_PIN_DISABLED;

static void debug_pin_setup(void)
{
  rcc_periph_ena(RCC_PERIPH_IOPC);
  gpio_setup(GPIO_PORT(CNF_DEBUG_PIN_PORT), CNF_DEBUG_PIN_PIN,
    GPIO_MODE_OUT_10_MHZ, GPIO_CNF_OUT_GP_OPEN_DRAIN);
  gpio_bit_set(GPIO_PORT(CNF_DEBUG_PIN_PORT), CNF_DEBUG_PIN_PIN);
  debug_pin_status = DEBUG_PIN_OFF;
}

static void debug_pin_on(void)
{
  if (debug_pin_status == DEBUG_PIN_DISABLED)
    return;

  gpio_odr_modify(GPIO_PORT(CNF_DEBUG_PIN_PORT), CNF_DEBUG_PIN_PIN, 0);
  debug_pin_status = DEBUG_PIN_ON;
}

static void debug_pin_off(void)
{
  if (debug_pin_status == DEBUG_PIN_DISABLED)
    return;

  gpio_odr_modify(GPIO_PORT(CNF_DEBUG_PIN_PORT), CNF_DEBUG_PIN_PIN, 1);
  debug_pin_status = DEBUG_PIN_OFF;
}

static void debug_pin_toggle(void)
{
  if (debug_pin_status == DEBUG_PIN_OFF) {
    debug_pin_on();
  } else if (debug_pin_status == DEBUG_PIN_ON) {
    debug_pin_off();
  }
}

