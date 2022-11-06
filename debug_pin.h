#pragma once

#define DEBUG_PIN_DISABLED 0
#define DEBUG_PIN_ON 1
#define DEBUG_PIN_OFF 2

static int debug_pin_status = DEBUG_PIN_DISABLED;

void debug_pin_setup(void)
{
  rcc_periph_ena(RCC_PERIPH_IOPC);
  gpioc_set_pin13();
  debug_pin_status = DEBUG_PIN_OFF;
}

void debug_pin_on(void)
{
  if (debug_pin_status == DEBUG_PIN_DISABLED)
    return;

  gpioc_bit_set(13);
  debug_pin_status = DEBUG_PIN_ON;
}

void debug_pin_off(void)
{
  if (debug_pin_status == DEBUG_PIN_DISABLED)
    return;

  gpioc_bit_clear(13);
  debug_pin_status = DEBUG_PIN_OFF;
}

void debug_pin_toggle(void)
{
  if (debug_pin_status == DEBUG_PIN_OFF) {
    debug_pin_on();
  } else if (debug_pin_status == DEBUG_PIN_ON) {
    debug_pin_off();
  }
}

