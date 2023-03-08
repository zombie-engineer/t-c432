#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <dma.h>

#define CALC_PSC(timeout, f_clk, auto_reload_value)  \
  (uint16_t)((float)f_clk * timeout / auto_reload_value - 1.0f)

int tim2_setup(bool one_pulse, uint16_t prescaler, uint16_t counter_value,
  bool enable_interrupt, bool enable);

uint16_t tim2_read_counter_value();

void tim3_setup(uint16_t prescaler, uint16_t period);

typedef enum {
  TIM3_CH1 = 0,
  TIM3_CH2 = 1,
  TIM3_CH3 = 2,
  TIM3_CH4 = 3,
  TIM3_CH_MAX = TIM3_CH4
} tim3_channel_id;

int tim3_pwm_dma_setup(tim3_channel_id ch);
void tim3_enable(void);
void tim3_disable(void);

void tim3_pwm_dma_run(int dma_channel, const void *src, int src_size);
