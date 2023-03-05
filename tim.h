#pragma once
#include <stdint.h>
#include <stdbool.h>

#define CALC_PSC(timeout, f_clk, auto_reload_value)  \
  (uint16_t)((float)f_clk * timeout / auto_reload_value - 1.0f)

int tim2_setup(bool one_pulse, uint16_t prescaler, uint16_t counter_value,
  bool enable_interrupt, bool enable);

uint16_t tim2_read_counter_value();
