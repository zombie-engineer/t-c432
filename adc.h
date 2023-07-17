#pragma once
#include <stdint.h>

void adc_setup_freerunning_dma(int gpio_port, int gpio_pin,
  uint16_t *dma_samples_buf, int dma_num_samples);
