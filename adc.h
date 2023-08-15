#pragma once
#include <stdint.h>
#include <stdbool.h>

#define ADC_CH_0  0
#define ADC_CH_1  1
#define ADC_CH_2  2
#define ADC_CH_3  3
#define ADC_CH_4  4
#define ADC_CH_5  5
#define ADC_CH_6  6
#define ADC_CH_6  6
#define ADC_CH_7  7
#define ADC_CH_8  8
#define ADC_CH_9  9
#define ADC_CH_10 10
#define ADC_CH_11 11
#define ADC_CH_12 12
#define ADC_CH_13 13
#define ADC_CH_14 14
#define ADC_CH_15 15
#define ADC_CH_16 16
#define ADC_CH_17 17

#define ADC_SAMPTIME_1_5_CYCLES   0
#define ADC_SAMPTIME_7_5_CYCLES   1
#define ADC_SAMPTIME_13_5_CYCLES  2
#define ADC_SAMPTIME_28_5_CYCLES  3
#define ADC_SAMPTIME_41_5_CYCLES  4
#define ADC_SAMPTIME_55_5_CYCLES  5
#define ADC_SAMPTIME_71_5_CYCLES  6
#define ADC_SAMPTIME_239_5_CYCLES 7

void adc_init(void);

struct adc_channel_addr {
  union {
    uint8_t adc_module : 3;
    uint8_t adc_channel: 5;
  } u;
  uint8_t value;
};

struct adc_channel_addr adc_get_temperature_channel(void);

struct adc_scan_mode_channel_config {
  /* ADC channel number */
  int idx;
  /* Sampling time in number of ADC clocks for 1 sampling */
  int sampling_time;
};

struct adc_scan_mode_config {
  const struct adc_scan_mode_channel_config *regular_group;
  int regular_group_size;

  const struct adc_scan_mode_channel_config *injected_group;
  int injected_group_size;
};

void adc_init_scan_mode(int adc_module,
  const struct adc_scan_mode_config *cfg);

void adc_setup_dma(uint16_t *dst_buf, unsigned int dst_buf_size);
void adc_run(void);

void adc_start_single_conversion_noirq(int adc_module, int adc_channel);

bool adc_is_conversion_finished(void);
uint16_t adc_get_last_data(void);
void adc_temperature_sensor_init(void);
void adc_setup_freerunning_dma(int gpio_port, int gpio_pin,
  uint16_t *dma_samples_buf, int dma_num_samples);

float adc_raw_data_to_temperature(uint16_t v_sense);
float adc_voltage_to_temperature(float voltage);
