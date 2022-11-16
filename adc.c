#include "adc.h"
#include "gpio.h"
#include "memory_layout.h"
#include "nvic.h"
#include "rcc.h"
#include "reg_access.h"

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

#define ADC_CR2_EON 0
#define ADC_CR1_EOCIE 5

static volatile int last_adc = 0;

void adc_setup(void)
{
  rcc_periph_ena(RCC_PERIPH_IOPA);
  rcc_periph_ena(RCC_PERIPH_ADC1);
  gpio_setup(GPIO_PORT_A, 1, GPIO_MODE_INPUT, GPIO_CNF_IN_ANALOG);

  reg_write(ADC1_CR2, 1 << ADC_CR2_EON);
  reg32_set_bit(ADC1_CR1, ADC_CR1_EOCIE);
  reg_write(ADC1_SR, 0);
  reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_ADC1);
  reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_ADC1);
  for (volatile int i = 0; i < 400; ++i)
  {
  }
  reg_write(ADC1_CR2, 1 << ADC_CR2_EON);
}

void adc_isr(void)
{
  last_adc = reg_read(ADC1_DR);
  // update_display();
  reg_write(ADC1_SR, 0);
  reg_write(ADC1_CR2, 1 << ADC_CR2_EON);
}

