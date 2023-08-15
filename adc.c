#include "adc.h"
#include "gpio.h"
#include "memory_layout.h"
#include "nvic.h"
#include "time.h"
#include "rcc.h"
#include "reg_access.h"
#include <svc.h>
#include <dma.h>
#include <common_util.h>
#include "config.h"

#define ADC_MODULE_IDX_ADC1 0
#define ADC_MODULE_IDX_ADC2 1

#define ADC1_SR    (volatile uint32_t *)(ADC1_BASE + 0x00)
#define ADC1_CR1   (volatile uint32_t *)(ADC1_BASE + 0x04)
#define ADC1_CR2   (volatile uint32_t *)(ADC1_BASE + 0x08)
#define ADC1_SMPR1 (volatile uint32_t *)(ADC1_BASE + 0x0c)
#define ADC1_SMPR2 (volatile uint32_t *)(ADC1_BASE + 0x10)
#define ADC_SMPR_1_5_CYCLES   0
#define ADC_SMPR_7_5_CYCLES   1
#define ADC_SMPR_13_5_CYCLES  2
#define ADC_SMPR_28_5_CYCLES  3
#define ADC_SMPR_41_5_CYCLES  4
#define ADC_SMPR_55_5_CYCLES  5
#define ADC_SMPR_71_5_CYCLES  6
#define ADC_SMPR_239_5_CYCLES 7
#define ADC1_JOFR0 (volatile uint32_t *)(ADC1_BASE + 0x14)
#define ADC1_JOFR1 (volatile uint32_t *)(ADC1_BASE + 0x18)
#define ADC1_JOFR2 (volatile uint32_t *)(ADC1_BASE + 0x1c)
#define ADC1_JOFR3 (volatile uint32_t *)(ADC1_BASE + 0x20)
#define ADC1_HTR   (volatile uint32_t *)(ADC1_BASE + 0x24)
#define ADC1_LTR   (volatile uint32_t *)(ADC1_BASE + 0x28)
#define ADC1_SQR1  (volatile uint32_t *)(ADC1_BASE + 0x2c)
#define ADC1_SQR2  (volatile uint32_t *)(ADC1_BASE + 0x30)
#define ADC1_SQR3  (volatile uint32_t *)(ADC1_BASE + 0x34)
#define ADC1_JSQR  (volatile uint32_t *)(ADC1_BASE + 0x38)
#define ADC1_JDR0  (volatile uint32_t *)(ADC1_BASE + 0x3c)
#define ADC1_JDR1  (volatile uint32_t *)(ADC1_BASE + 0x40)
#define ADC1_JDR2  (volatile uint32_t *)(ADC1_BASE + 0x44)
#define ADC1_JDR3  (volatile uint32_t *)(ADC1_BASE + 0x48)
#define ADC1_DR    (volatile uint32_t *)(ADC1_BASE + 0x4c)

#define ADC_CR1_AWDCH 0
#define ADC_CR1_AWDCH_WIDTH 5
#define ADC_CR1_EOCIE 5
#define ADC_CR1_EOCIE_WIDTH 1
#define ADC_CR1_AWDIE 6
#define ADC_CR1_AWDIE_WIDTH 1
#define ADC_CR1_JEOCIE 7
#define ADC_CR1_JEOCIE_WIDTH 1
#define ADC_CR1_SCAN 8
#define ADC_CR1_SCAN_WIDTH 1
#define ADC_CR1_AWDSGL 9
#define ADC_CR1_AWDSGL_WIDTH 1
#define ADC_CR1_JAUTO 10
#define ADC_CR1_JAUTO_WIDTH 1
#define ADC_CR1_DISCEN 11
#define ADC_CR1_DISCEN_WIDTH 1
#define ADC_CR1_JDISCEN 12
#define ADC_CR1_JDISCEN_WIDTH 1
#define ADC_CR1_DISCNUM 13
#define ADC_CR1_DISCNUM_WIDTH 3
#define ADC_CR1_DUALMOD 16
#define ADC_CR1_DUALMOD_WIDTH 4
#define ADC_CR1_JAUDEN 22
#define ADC_CR1_JAUDEN_WIDTH 1
#define ADC_CR1_AWDEN 23
#define ADC_CR1_AWDEN_WIDTH 1

#define ADC_CR2_ADON 0
#define ADC_CR2_ADON_WIDTH 1
#define ADC_CR2_CONT 1
#define ADC_CR2_CONT_WIDTH 1
#define ADC_CR2_CAL 2
#define ADC_CR2_CAL_WIDTH 1
#define ADC_CR2_RSTCAL 3
#define ADC_CR2_RSTCAL_WIDTH 1
#define ADC_CR2_DMA 8
#define ADC_CR2_DMA_WIDTH 1
#define ADC_CR2_ALIGN 11
#define ADC_CR2_ALIGN_WIDTH 1
#define ADC_CR2_JEXTSEL 12
#define ADC_CR2_JEXTSEL_WIDTH 3
#define ADC_CR2_JEXTTRIG 15
#define ADC_CR2_JEXTTRIG_WIDTH 1
#define ADC_CR2_EXTSEL 17
#define ADC_CR2_EXTSEL_WIDTH 3
#define ADC_CR2_EXTTRIG 20
#define ADC_CR2_EXTTRIG_WIDTH 1
#define ADC_CR2_JSWSTART 21
#define ADC_CR2_JSWSTART_WIDTH 1
#define ADC_CR2_SWSTART 22
#define ADC_CR2_SWSTART_WIDTH 1
#define ADC_CR2_TSVREFE 23
#define ADC_CR2_TSVREFE_WIDTH 1

#define ADC_SQR3_L 20
#define ADC_SQR3_L_WIDTH 4

#define ADC_SR_EOC 1
#define ADC1_CH_TEMPERATURE 16
#define ADC_REGULAR_GROUP_SIZE 16

static int adc_buf_idx = 0;
static int adc_prescaler = 0;
static int adc_dma_channel = 0;

void adc_init(void)
{
  if (!reg32_bit_is_set(ADC1_CR2, ADC_CR2_ADON)) {
    uint32_t reg = reg_read(ADC1_CR2);
    u32_set_bit(&reg, ADC_CR2_ADON);
#if defined ADC_INT_TEMP_SENSOR_ENABLE
    u32_set_bit(&reg, ADC_CR2_TSVREFE);
#endif
    reg_write(ADC1_CR2, reg);
    wait_ms(50);
  }

  /* Do calibration */
  reg32_set_bit(ADC1_CR2, ADC_CR2_CAL);
  /* CAL bit is reset by hardware after calibration is complete */
  while(reg32_bit_is_set(ADC1_CR2, ADC_CR2_CAL));
}

void adc_dma_complete(void)
{
}

struct adc_channel_addr adc_get_temperature_channel(void)
{
  struct adc_channel_addr ret = {
    .u = {
      .adc_module = ADC_MODULE_IDX_ADC1,
      .adc_channel = ADC1_CH_TEMPERATURE
    }
  };
  return ret;
}

bool adc_is_conversion_finished(void)
{
  return reg32_bit_is_set(ADC1_SR, ADC_SR_EOC);
}

uint16_t adc_get_last_data(void)
{
  return reg_read(ADC1_DR);
}

static void adc_set_sample_time(int adc_module, int adc_channel,
  int sample_time)
{
  int pos;
  reg32_t smpr = ADC1_SMPR2;
  if (adc_channel > 9)
    smpr--;

  pos = (adc_channel % 10) * 3;
  reg32_modify_bits(smpr, pos, 3, sample_time);
}

void adc_temperature_sensor_init(void)
{
  reg32_set_bit(ADC1_CR2, ADC_CR2_TSVREFE);

  /*
   * Select sampling time close to 17.1 usec
   * ADCCLK = 12Mhz, 12 clocks = 1usec,
   * 17.1 = 12 * 17.1 = 205.2
   */
  adc_set_sample_time(ADC_MODULE_IDX_ADC1, ADC1_CH_TEMPERATURE,
    ADC_SMPR_239_5_CYCLES);
}

void adc_start_single_conversion_noirq(int adc_module, int adc_channel)
{
  /* Select regular channel for conversion */
  reg_write(ADC1_SQR3, adc_channel);
  reg32_set_bit(ADC1_CR2, ADC_CR2_ADON);
}

float adc_raw_data_to_temperature(uint16_t raw)
{
  float v_sense = (raw / 4095.0f) * 3.3;
  const float avg_slope = 2;
  const float v_25 = 1.43f;
  return (v_sense - v_25) * avg_slope + 25;
}

float adc_voltage_to_temperature(float voltage)
{
  const float v_25 = 1.65;
  return (v_25 - voltage) * 130 + 25;
}

void adc_init_scan_mode(int adc_module, const struct adc_scan_mode_config *cfg)
{
  int i;
  int reg_idx;
  int reg_offset;
  int num_conversions = min(ADC_REGULAR_GROUP_SIZE, cfg->regular_group_size);
  const struct adc_scan_mode_channel_config *ch;
  uint32_t smpr[2] = { 0 };
  uint32_t sqr[3] = { 0 };
  uint32_t jsqr = 0;

  for (i = 0; i < num_conversions; ++i) {
    ch = &cfg->regular_group[i];
    reg_idx = 2 - i / 6;
    reg_offset = (i % 6) * 5;
    sqr[reg_idx] |= (ch->idx & 0x1f) << reg_offset;

    reg_idx = 1 - i / 10;
    reg_offset = (i % 10) * 3;
    smpr[reg_idx] |= (ch->sampling_time & 7) << reg_offset;
  }

  u32_modify_bits(&sqr[0], ADC_SQR3_L, ADC_SQR3_L_WIDTH, num_conversions - 1);

  for (i = 0; i < cfg->injected_group_size; ++i) {
    ch = &cfg->injected_group[i];
    jsqr |= (ch->idx << (i * 5));
  }

  for (i = 0; i < ARRAY_SIZE(sqr); ++i)
    reg_write(ADC1_SQR1 + i, sqr[i]);

  for (i = 0; i < ARRAY_SIZE(smpr); ++i)
    reg_write(ADC1_SMPR1 + i, smpr[i]);

  reg_write(ADC1_JSQR, jsqr);

  /* Clear status register */
  reg_write(ADC1_SR, 0);
  reg32_set_bit(ADC1_CR1, ADC_CR1_SCAN);
  reg32_set_bit(ADC1_CR2, ADC_CR2_CONT);
}

void adc_setup_dma(uint16_t *dst_buf, unsigned int dst_buf_size)
{
  reg32_set_bit(ADC1_CR2, ADC_CR2_DMA);

  adc_dma_channel = dma_get_channel_id(DMA_PERIPH_ADC1);

  struct dma_channel_settings dma_settings = {
    .paddr = ADC1_DR,
    .maddr = dst_buf,
    .count = dst_buf_size,
    .dir = DMA_TRANSFER_DIR_FROM_PERIPH,
    .pwidth = 16,
    .mwidth = 16,
    .circular = true,
    .pinc = false,
    .minc = true,
    .interrupt_on_completion = false,
    .enable_after_setup = false
  };

  dma_transfer_setup(adc_dma_channel, &dma_settings);
  dma_transfer_enable(adc_dma_channel);
}

void adc_run(void)
{
  reg32_set_bit(ADC1_CR2, ADC_CR2_ADON);
}

void adc_setup_freerunning_dma(int gpio_port, int gpio_pin,
  uint16_t *dma_adc_array, int dma_adc_array_length)
{
  /* Start ADC1 clocks */
  rcc_periph_ena(RCC_PERIPH_ADC1);

  /* Set up GPIO pin to ADC1 function */
  gpio_setup(gpio_port, gpio_pin, GPIO_MODE_INPUT, GPIO_CNF_IN_ANALOG);


  /* Clear status register */
  reg_write(ADC1_SR, 0);

  /* Clear any pending interrupts and only after enable them */
  nvic_clear_pending(NVIC_INTERRUPT_NUMBER_ADC1);
  nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_ADC1);

  /* Enable DMA */
  reg32_set_bit(ADC1_CR2, ADC_CR2_DMA);

  reg32_set_bit(ADC1_CR2, ADC_CR2_CONT);

  adc_dma_channel = dma_get_channel_id(DMA_PERIPH_ADC1);
  dma_enable_interrupt(DMA_NUM_1, adc_dma_channel);
  dma_set_isr_cb(1, adc_dma_channel, adc_dma_complete);

  struct dma_channel_settings dma_settings = {
    .paddr = ADC1_DR,
    .maddr = dma_adc_array,
    .count = dma_adc_array_length,
    .dir = DMA_TRANSFER_DIR_FROM_PERIPH,
    .pwidth = 16,
    .mwidth = 16,
    .circular = true,
    .pinc = false,
    .minc = true,
    .interrupt_on_completion = true,
    .enable_after_setup = false
  };

  dma_transfer_setup(adc_dma_channel, &dma_settings);
  dma_transfer_enable(adc_dma_channel);

  /*
   * ADON bit is set twice:
   * - first time to power up ADC module, it will then hold value 1 for ADON
   *    bit
   * - second time some to start conversion, 1 is written on top of existing
   *   1 after time tSTAB has passed since first time,
   * See STMF103xx advanced Arm-based 32-bit MCUs - Reference manual, page 243
   * ADON bit is set alone, otherwise conversion will not start
   * SWSTART is set AFTER ADON
   */
  reg32_set_bit(ADC1_CR2, ADC_CR2_ADON);
  reg32_set_bit(ADC1_CR2, ADC_CR2_SWSTART);
}

void __adc_isr(void)
{
}
