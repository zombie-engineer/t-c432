#pragma once
#include <stdint.h>
#include "reg_access.h"

void dma_init(void);

typedef enum {
  DMA_NUM_UNKNOWN,
  DMA_NUM_1,
  DMA_NUM_2,
} dma_num_t;

void dma_enable_interrupt(dma_num_t dma, int ch);

typedef enum {
  DMA_PERIPH_ADC1,
  DMA_PERIPH_TIM1_TRIG,
  DMA_PERIPH_TIM1_COM,
  DMA_PERIPH_TIM1_UP,
  DMA_PERIPH_TIM1_CH1,
  DMA_PERIPH_TIM1_CH2,
  DMA_PERIPH_TIM1_CH3,
  DMA_PERIPH_TIM1_CH4,
  DMA_PERIPH_TIM2_UP,
  DMA_PERIPH_TIM2_CH1,
  DMA_PERIPH_TIM2_CH2,
  DMA_PERIPH_TIM2_CH3,
  DMA_PERIPH_TIM2_CH4,
  DMA_PERIPH_TIM3_TRIG,
  DMA_PERIPH_TIM3_UP,
  DMA_PERIPH_TIM3_CH1,
  DMA_PERIPH_TIM3_CH3,
  DMA_PERIPH_TIM3_CH4,
  DMA_PERIPH_TIM4_UP,
  DMA_PERIPH_TIM4_CH1,
  DMA_PERIPH_TIM4_CH2,
  DMA_PERIPH_TIM4_CH3,
  DMA_PERIPH_USART1_RX,
  DMA_PERIPH_USART1_TX,
  DMA_PERIPH_USART2_RX,
  DMA_PERIPH_USART2_TX,
  DMA_PERIPH_USART3_RX,
  DMA_PERIPH_USART3_TX,
  DMA_PERIPH_SPI1_RX,
  DMA_PERIPH_SPI1_TX,
  DMA_PERIPH_SPI2_RX,
  DMA_PERIPH_SPI2_TX,
  DMA_PERIPH_I2C1_RX,
  DMA_PERIPH_I2C1_TX,
  DMA_PERIPH_I2C2_RX,
  DMA_PERIPH_I2C2_TX,
  DMA_PERIPH_COUNT
} dma_periph_t;

int dma_get_channel_id(dma_periph_t p);

void dma_transfer_setup(int dma_ch, volatile void *paddr, void *maddr,
  int size, int mwidth, int pwidth, bool minc, bool pinc, bool enable,
  bool interrupt_on_completion);

void dma_transfer_enable(int ch);
void dma_transfer_disable(int ch);

typedef void(*dma_isr_cb)(void);
void dma_set_isr_cb(dma_num_t dma, int ch, dma_isr_cb cb);
