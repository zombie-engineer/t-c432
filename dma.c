#include "dma.h"
#include "memory_layout.h"
#include "reg_access.h"
#include "nvic.h"
#include "rcc.h"
#include "common_util.h"

#define NUM_DMA1_CHANNELS 7
#define NUM_DMA2_CHANNELS 5

/* DMA1 and DMA2 base addresses are this far from each other */
#define DMA_REGSPACE_SIZE 0x400

/*
 * DMA per-channel registers are this far from each other,
 * ex (CCR2 is this far from CCR1)
 */
#define DMA_CH_REGSPACE_SIZE 0x14
#define CHOFF(__ch, __off) ((__ch) * DMA_CH_REGSPACE_SIZE + (__off))

/* returns base address for DMAn, given n */
#define DMA_BASE(__dma) (DMA1_BASE + (__dma * DMA_REGSPACE_SIZE))
/* helper definition that defines one per-channel DMA register */
#define DMA_CH_REG(__dma, __ch, __off) \
  (reg32_t)(DMA_BASE(__dma) + CHOFF((__ch), (__off)))

/* ISR and IFSR are common to all channels of a single DMA module */
#define DMA_ISR(__dma) (reg32_t)(DMA_BASE(__dma) + 0x00)
#define DMA_IFSR(__dma) (reg32_t)(DMA_BASE(__dma) + 0x04)

/* These are per-channel DMA registes */
#define DMA_CCR(__dma, __ch)   DMA_CH_REG((__dma), (__ch), 0x08)
#define DMA_CNDTR(__dma, __ch) DMA_CH_REG((__dma), (__ch), 0x0c)
#define DMA_CPAR(__dma, __ch)  DMA_CH_REG((__dma), (__ch), 0x10)
#define DMA_CMAR(__dma, __ch)  DMA_CH_REG((__dma), (__ch), 0x14)

#define DMA_CCR_EN 0
#define DMA_CCR_TCIE 1
#define DMA_CCR_HTIE 2
#define DMA_CCR_TEIE 3
#define DMA_CCR_DIR 4
#define DMA_CCR_CIRC 5
#define DMA_CCR_PINC 6
#define DMA_CCR_MINC 7
#define DMA_CCR_PSIZE 8
#define DMA_CCR_PSIZE_WIDTH 2
#define DMA_CCR_PSIZE_8_BITS 0b00
#define DMA_CCR_PSIZE_16_BITS 0b01
#define DMA_CCR_PSIZE_32_BITS 0b10
#define DMA_CCR_MSIZE 10
#define DMA_CCR_MSIZE_WIDTH 2
#define DMA_CCR_MSIZE_8_BITS 0b00
#define DMA_CCR_MSIZE_16_BITS 0b01
#define DMA_CCR_MSIZE_32_BITS 0b10
#define DMA_CCR_PL 12
#define DMA_CCR_PL_WIDTH 2
#define DMA_CCR_PL_MEM2MEM 14

static dma_isr_cb dma1_isr_cbs[NUM_DMA1_CHANNELS] = { 0 };
static dma_isr_cb dma2_isr_cbs[NUM_DMA2_CHANNELS] = { 0 };

void dma_enable_interrupt(dma_num_t dma, int ch)
{
  if (dma == DMA_NUM_1) {
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_DMA1_CHAN_1 + ch);
  } else if (dma == DMA_NUM_2) {
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_DMA2_CHAN_1 + ch);
  }
}

void dma_transfer_disable(int ch)
{
  reg32_clear_bit(DMA_CCR(0, ch), DMA_CCR_EN);
}

void dma_transfer_enable(int ch)
{
  reg32_set_bit(DMA_CCR(0, ch), DMA_CCR_EN);
}

void dma_transfer_setup(int ch, const struct dma_channel_settings *s)
{
  uint32_t v = 0;
  int width;

  reg_write(DMA_CPAR(0, ch), (uint32_t)s->paddr);
  reg_write(DMA_CMAR(0, ch), (uint32_t)s->maddr);
  reg_write(DMA_CNDTR(0, ch), s->count);
  /* transfer complete interrupt enable */

  reg_write(DMA_CCR(0, ch), 0);

  /* transfer error interrupt enable */
  u32_set_bit(&v, DMA_CCR_TEIE);

  if (s->interrupt_on_completion)
    u32_set_bit(&v, DMA_CCR_TCIE);

  if (s->dir == DMA_TRANSFER_DIR_TO_PERIPH) {
    /* DIR=1 is "Read from memory" */
    u32_set_bit(&v, DMA_CCR_DIR);
  } else {
    u32_clear_bit(&v, DMA_CCR_DIR);
  }

  if (s->circular)
    u32_set_bit(&v, DMA_CCR_CIRC);

  /* Memory increment mode, peripheral address not incremented */
  if (s->minc)
    u32_set_bit(&v, DMA_CCR_MINC);

  if (s->pinc)
    u32_set_bit(&v, DMA_CCR_PINC);

  width = DMA_CCR_MSIZE_8_BITS;
  if (s->mwidth == 16)
    width = DMA_CCR_MSIZE_16_BITS;
  else if (s->mwidth == 32)
    width = DMA_CCR_MSIZE_32_BITS;

  /* Peripheral data width */
  u32_modify_bits(&v, DMA_CCR_MSIZE, DMA_CCR_MSIZE_WIDTH,
    width);

  width = DMA_CCR_PSIZE_8_BITS;
  if (s->pwidth == 16)
    width = DMA_CCR_PSIZE_16_BITS;
  else if (s->pwidth == 32)
    width = DMA_CCR_PSIZE_32_BITS;

  /* Memory data width */
  u32_modify_bits(&v, DMA_CCR_PSIZE, DMA_CCR_PSIZE_WIDTH,
    width);

  if (s->enable_after_setup)
    u32_set_bit(&v, DMA_CCR_EN);

  reg_write(DMA_CCR(0, ch), v);
}

int dma_get_channel_id(dma_periph_t p)
{
  static const char map[] = {
    [DMA_PERIPH_ADC1] = 0,
    [DMA_PERIPH_TIM1_TRIG] = 4,
    [DMA_PERIPH_TIM1_COM] = 4,
    [DMA_PERIPH_TIM1_UP] = 5,
    [DMA_PERIPH_TIM1_CH1] = 2,
    /* TIM1_CH2 is not correctly documented, so this is not 100% */
    [DMA_PERIPH_TIM1_CH2] = 3,
    [DMA_PERIPH_TIM1_CH3] = 6,
    [DMA_PERIPH_TIM1_CH4] = 4,
    [DMA_PERIPH_TIM2_UP] = 2,
    [DMA_PERIPH_TIM2_CH1] = 5,
    [DMA_PERIPH_TIM2_CH2] = 7,
    [DMA_PERIPH_TIM2_CH3] = 1,
    [DMA_PERIPH_TIM2_CH4] = 7,
    [DMA_PERIPH_TIM3_TRIG] = 6,
    [DMA_PERIPH_TIM3_UP] = 3,
    [DMA_PERIPH_TIM3_CH1] = 6,
    [DMA_PERIPH_TIM3_CH3] = 2,
    [DMA_PERIPH_TIM3_CH4] = 3,
    [DMA_PERIPH_TIM4_UP] = 7,
    [DMA_PERIPH_TIM4_CH1] = 1,
    [DMA_PERIPH_TIM4_CH2] = 4,
    [DMA_PERIPH_TIM4_CH3] = 5,
    [DMA_PERIPH_USART1_RX] = 5,
    [DMA_PERIPH_USART1_TX] = 4,
    [DMA_PERIPH_USART2_RX] = 6,
    [DMA_PERIPH_USART2_TX] = 7,
    [DMA_PERIPH_USART3_RX] = 3,
    [DMA_PERIPH_USART3_TX] = 2,
    [DMA_PERIPH_SPI1_RX] = 2,
    [DMA_PERIPH_SPI1_TX] = 3,
    [DMA_PERIPH_SPI2_RX] = 4,
    [DMA_PERIPH_SPI2_TX] = 5,
    [DMA_PERIPH_I2C1_RX] = 7,
    [DMA_PERIPH_I2C1_TX] = 6,
    [DMA_PERIPH_I2C2_RX] = 5,
    [DMA_PERIPH_I2C2_TX] = 4,
  };

  if (p >= ARRAY_SIZE(map))
    return 0;

  return map[p];
}

void dma_set_isr_cb(dma_num_t dma, int ch, dma_isr_cb cb)
{
  if (dma = DMA_NUM_1 && ch < NUM_DMA1_CHANNELS) {
    dma1_isr_cbs[ch] = cb;
  } else if (dma == DMA_NUM_2 && ch < NUM_DMA2_CHANNELS) {
    dma2_isr_cbs[ch] = cb;
  }
}

void dma_isr(int dma_idx, int ch_idx)
{
  dma_isr_cb cb = NULL;

  reg32_write_clear_bit(DMA_IFSR(dma_idx), ch_idx * 4);
  reg32_write_clear_bit(DMA_IFSR(dma_idx), ch_idx * 4 + 1);
  reg32_write_clear_bit(DMA_IFSR(dma_idx), ch_idx * 4 + 2);
  reg32_write_clear_bit(DMA_IFSR(dma_idx), ch_idx * 4 + 3);

  if (dma_idx == 0 && ch_idx < NUM_DMA1_CHANNELS) {
    cb = dma1_isr_cbs[ch_idx];

  } else if (dma_idx == 1 && ch_idx < NUM_DMA2_CHANNELS) {
    cb = dma2_isr_cbs[ch_idx];
  } else {
    asm volatile("bkpt");
    return;
  }

  if (cb)
    cb();
}

void dma_init(void)
{
  rcc_periph_ena(RCC_PERIPH_DMA1);
}
