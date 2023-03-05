#include "tim.h"
#include "reg_access.h"
#include "memory_layout.h"
#include "rcc.h"
#include "nvic.h"
#include "config.h"
#include "dma.h"

#define TIM2_CR1   (volatile uint32_t *)(TIM2_BASE + 0x00)
#define TIM2_CR2   (volatile uint32_t *)(TIM2_BASE + 0x04)
#define TIM2_SMCR  (volatile uint32_t *)(TIM2_BASE + 0x08)
#define TIM2_DIER  (volatile uint32_t *)(TIM2_BASE + 0x0c)
#define TIM2_SR    (volatile uint32_t *)(TIM2_BASE + 0x10)
#define TIM2_EGR   (volatile uint32_t *)(TIM2_BASE + 0x14)
#define TIM2_CCMR1 (volatile uint32_t *)(TIM2_BASE + 0x18)
#define TIM2_CCMR2 (volatile uint32_t *)(TIM2_BASE + 0x1c)
#define TIM2_CCER  (volatile uint32_t *)(TIM2_BASE + 0x20)
#define TIM2_CNT   (volatile uint32_t *)(TIM2_BASE + 0x24)
#define TIM2_PSC   (volatile uint32_t *)(TIM2_BASE + 0x28)
#define TIM2_ARR   (volatile uint32_t *)(TIM2_BASE + 0x2c)
#define TIM2_CCR1  (volatile uint32_t *)(TIM2_BASE + 0x34)
#define TIM2_CCR2  (volatile uint32_t *)(TIM2_BASE + 0x38)
#define TIM2_CCR3  (volatile uint32_t *)(TIM2_BASE + 0x3c)
#define TIM2_CCR4  (volatile uint32_t *)(TIM2_BASE + 0x40)
#define TIM2_DCR   (volatile uint32_t *)(TIM2_BASE + 0x48)
#define TIM2_DMAR  (volatile uint32_t *)(TIM2_BASE + 0x4c)

#define TIM3_CR1   (volatile uint32_t *)(TIM3_BASE + 0x00)
#define TIM3_CR2   (volatile uint32_t *)(TIM3_BASE + 0x04)
#define TIM3_SMCR  (volatile uint32_t *)(TIM3_BASE + 0x08)
#define TIM3_DIER  (volatile uint32_t *)(TIM3_BASE + 0x0c)
#define TIM3_SR    (volatile uint32_t *)(TIM3_BASE + 0x10)
#define TIM3_EGR   (volatile uint32_t *)(TIM3_BASE + 0x14)
#define TIM3_CCMR1 (volatile uint32_t *)(TIM3_BASE + 0x18)
#define TIM3_CCMR2 (volatile uint32_t *)(TIM3_BASE + 0x1c)

#define TIMx_CCMR1_OC1M 4
#define TIMx_CCMR1_OC1M_WIDTH 3
#define TIMx_CCMR1_OC2M 12
#define TIMx_CCMR1_OC2M_WIDTH TIMx_CCMR1_OC1M_WIDTH

#define TIMx_CCMR2_OC3M TIMx_CCMR1_OC1M
#define TIMx_CCMR2_OC3M_WIDTH TIMx_CCMR1_OC1M_WIDTH
#define TIMx_CCMR2_OC4M TIMx_CCMR1_OC2M
#define TIMx_CCMR2_OC4M_WIDTH TIMx_CCMR1_OC1M_WIDTH

#define TIMx_CCMRx_OCxM_FROZEN  0b000
#define TIMx_CCMRx_OCxM_MATCH   0b001
#define TIMx_CCMRx_OCxM_NOMATCH 0b010
#define TIMx_CCMRx_OCxM_TOGGLE  0b011
#define TIMx_CCMRx_OCxM_FORCE0  0b100
#define TIMx_CCMRx_OCxM_FORCE1  0b101
#define TIMx_CCMRx_OCxM_PWM1    0b110
#define TIMx_CCMRx_OCxM_PWM2    0b111

#define TIMx_CCRM1_OC1PE 3
#define TIMx_CCRM1_OC2PE (TIMx_CCRM1_OC1PE + 8)
#define TIMx_CCRM2_OC3PE TIMx_CCRM1_OC1PE
#define TIMx_CCRM2_OC4PE TIMx_CCRM1_OC2PE
#define TIMx_CCRMx_OCxPE_PRELOAD 1
#define TIMx_CCRMx_OCxPE_NO_PRELOAD 0


#define TIM3_CCER  (volatile uint32_t *)(TIM3_BASE + 0x20)
#define TIMx_CCER_CC1E 0
#define TIMx_CCER_CC1P 1
#define TIMx_CCER_CC2E 4
#define TIMx_CCER_CC2P 5
#define TIMx_CCER_CC3E 8
#define TIMx_CCER_CC3P 9
#define TIMx_CCER_CC4E 12
#define TIMx_CCER_CC4P 13
#define TIMx_CCER_CCxE_ON 1
#define TIMx_CCER_CCxE_OFF 0

#define TIM3_CNT   (volatile uint32_t *)(TIM3_BASE + 0x24)
#define TIM3_PSC   (volatile uint32_t *)(TIM3_BASE + 0x28)
#define TIM3_ARR   (volatile uint32_t *)(TIM3_BASE + 0x2c)
#define TIM3_CCR1  (volatile uint32_t *)(TIM3_BASE + 0x34)
#define TIM3_CCR2  (volatile uint32_t *)(TIM3_BASE + 0x38)
#define TIM3_CCR3  (volatile uint32_t *)(TIM3_BASE + 0x3c)
#define TIM3_CCR4  (volatile uint32_t *)(TIM3_BASE + 0x40)
#define TIM3_DCR   (volatile uint32_t *)(TIM3_BASE + 0x48)
#define TIM3_DMAR  (volatile uint32_t *)(TIM3_BASE + 0x4c)

#define TIMx_CEN  (1<<0)
#define TIMx_UDIS (1<<1)
#define TIMx_URS  (1<<2)
#define TIMx_OPM  (1<<3)
#define TIMx_DIR  (1<<4)
#define TIMx_CMS  (3<<5)
#define TIMx_ARPE (1<<7)
#define TIMx_CKD  (3<<8)

#define TIMx_DIER_UIE 0
#define TIMx_DIER_CC1IE 1
#define TIMx_DIER_CC2IE 2
#define TIMx_DIER_CC3IE 3
#define TIMx_DIER_CC4IE 4
#define TIMx_DIER_COMIE 5
#define TIMx_DIER_TIE 6
#define TIMx_DIER_BIE 7
#define TIMx_DIER_UDE 8
#define TIMx_DIER_CC1DE 9
#define TIMx_DIER_CC2DE 10
#define TIMx_DIER_CC3DE 11
#define TIMx_DIER_CC4DE 12
#define TIMx_DIER_COMDE 13
#define TIMx_DIER_TDE 14

void led_strip_timer_setup(const void *mem, int memsz)
{
  uint32_t v;
  int dma_channel;
  dma_init();

  rcc_periph_ena(RCC_PERIPH_TIM3);

  reg_write(TIM3_CNT, 0);
  reg_write(TIM3_PSC, 1);
  reg_write(TIM3_ARR, LED_TIM_PERIOD - 1);

  reg_write(TIM3_CCR3, 0);

  reg_write(TIM3_CCMR2,
    (TIMx_CCMRx_OCxM_PWM1 << TIMx_CCMR2_OC3M)
    | (TIMx_CCRMx_OCxPE_PRELOAD << TIMx_CCRM2_OC3PE));

  reg_write(TIM3_CCER, TIMx_CCER_CCxE_ON << TIMx_CCER_CC3E);

  reg_write(TIM3_DIER, 1 << TIMx_DIER_UDE);
  reg_write(TIM3_DCR, (TIM3_CCR3 - TIM3_CR1) & 0x1f);

  reg_write(TIM3_CR1, TIMx_CEN | TIMx_ARPE);

  dma_channel = dma_get_channel_id(DMA_PERIPH_TIM3_CH3);
  dma_transfer_setup(dma_channel, TIM3_DMAR, mem, memsz, 16, 16, true, false,
    true);
}

int tim2_setup(bool one_pulse, uint16_t prescaler, uint16_t counter_value,
  bool enable_interrupt, bool enable)
{
  uint16_t v;

  reg_write(TIM2_ARR, counter_value);
  reg_write(TIM2_PSC, prescaler);

  v = reg_read(TIM2_CR1);

  /* Auto-reload */
  v |= TIMx_ARPE;

  /* center-aligned off */
  v &= ~TIMx_CMS;

  /* count dir = up */
  v &= ~TIMx_DIR;

  if (one_pulse)
    v |= TIMx_OPM;
  else
    v &= ~TIMx_OPM;

  /* Update only from overflow */
  v |= TIMx_URS;

  /* Enable interrupt for TIM2 */
  if (enable_interrupt) {
    /* Event on overflow enabled */
    reg_write(TIM2_SR, 0);
    reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
    v &= TIMx_UDIS;
    reg_write(TIM2_DIER, 1);
    reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
  }

  if (enable) {
    v |= TIMx_CEN;;
  }

  reg_write(TIM2_CR1, v);
  
  return 0;
}

void timer_setup(void)
{
  /* SYSCLK = 72MHz */
  rcc_periph_ena(RCC_PERIPH_TIM2);
  tim2_setup(true, CALC_PSC(0.1, F_CPU, 0xffff), 0xffff, true, true);
}

uint16_t tim2_read_counter_value(void)
{
  return reg_read(TIM2_CNT);
}

void __attribute__((weak)) tim2_isr_cb(void)
{
}

void __tim2_isr(void)
{
  reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
  reg_write(TIM2_SR, 0);
  reg32_set_bit(TIM2_CR1, 0);
  tim2_isr_cb();
}

