#include "adc.h"
#include "gpio.h"
#include "memory_layout.h"
#include "nvic.h"
#include "rcc.h"
#include "reg_access.h"
#include <svc.h>
#include <dma.h>

#define ADC1_SR    (volatile uint32_t *)(ADC1_BASE + 0x00)
#define ADC1_CR1   (volatile uint32_t *)(ADC1_BASE + 0x04)
#define ADC1_CR2   (volatile uint32_t *)(ADC1_BASE + 0x08)
#define ADC1_SMPR1 (volatile uint32_t *)(ADC1_BASE + 0x0c)
#define ADC1_SMPR2 (volatile uint32_t *)(ADC1_BASE + 0x10)
#define ADC1_JOFR0 (volatile uint32_t *)(ADC1_BASE + 0x14)
#define ADC1_JOFR1 (volatile uint32_t *)(ADC1_BASE + 0x18)
#define ADC1_JOFR2 (volatile uint32_t *)(ADC1_BASE + 0x1c)
#define ADC1_JOFR3 (volatile uint32_t *)(ADC1_BASE + 0x20)
#define ADC1_HTR   (volatile uint32_t *)(ADC1_BASE + 0x24)
#define ADC1_LTR   (volatile uint32_t *)(ADC1_BASE + 0x28)
#define ADC1_JSQR  (volatile uint32_t *)(ADC1_BASE + 0x2c)
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

#define ADC_SR_EOC 1

#define ADC_BUF_SIZE 128
uint16_t adc_buf[ADC_BUF_SIZE];
static int adc_buf_idx = 0;
static int adc_prescaler = 0;
static int adc_dma_channel = 0;

void adc_dma_complete(void)
{
#if 0
  dma_transfer_setup(adc_dma_channel, ADC1_DR, adc_buf,
    DMA_TRANSFER_DIR_FROM_PERIPH,
    128, 16, 16,
    true /* memory address increment */,
    false /* periph address increment */,
    true /* enable */,
    true /* interrupt_on_completion */);
#endif
}

void adc_setup(void)
{
  rcc_periph_ena(RCC_PERIPH_ADC1);
  gpio_setup(GPIO_PORT_A, 0, GPIO_MODE_INPUT, GPIO_CNF_IN_ANALOG);
  dma_init();

  reg_write(ADC1_SR, 0);
  nvic_clear_pending(NVIC_INTERRUPT_NUMBER_ADC1);
  nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_ADC1);

//  reg32_set_bit(ADC1_CR1, ADC_CR1_EOCIE);
  reg32_set_bit(ADC1_CR2, ADC_CR2_DMA);
  reg32_set_bit(ADC1_CR2, ADC_CR2_CONT);
  
  adc_dma_channel = dma_get_channel_id(DMA_PERIPH_ADC1);
  dma_enable_interrupt(DMA_NUM_1, adc_dma_channel);
  dma_set_isr_cb(1, adc_dma_channel, adc_dma_complete);

  struct dma_channel_settings dma_settings = {
    .paddr = ADC1_DR,
    .maddr = adc_buf,
    .count = ADC_BUF_SIZE,
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

  reg32_set_bit(ADC1_CR2, ADC_CR2_ADON);
  svc_wait_ms(50);
  reg32_set_bit(ADC1_CR2, ADC_CR2_ADON);
  reg32_set_bit(ADC1_CR2, ADC_CR2_SWSTART);

#if 0
  while(1) {
    reg32_set_bit(ADC1_CR2, ADC_CR2_ADON);
    while(!reg32_bit_is_set(ADC1_SR, ADC_SR_EOC));
    adc_prescaler++;
    if (adc_prescaler < 20)
      continue;

    prescaler = 0;

    adc_buf[adc_buf_idx++] = reg_read(ADC1_DR);
    if (adc_buf_idx == 128) {
      adc_buf_idx = 0;
    }
  }
#endif
}

void __adc_isr(void)
{
  // last_adc = reg_read(ADC1_DR);
  // reg_write(ADC1_SR, 0);
  // reg32_set_bit(ADC1_CR2, ADC_CR2_ADON);
}

