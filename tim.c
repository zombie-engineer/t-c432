#include "tim.h"
#include "reg_access.h"
#include "memory_layout.h"
#include "nvic.h"

#define TIM2_CR1   (volatile uint32_t *)(TIM2_BASE + 0x00)
#define TIM2_CR2   (volatile uint32_t *)(TIM2_BASE + 0x04)
#define TIM2_SMCR  (volatile uint32_t *)(TIM2_BASE + 0x08)
#define TIM2_DIER  (volatile uint32_t *)(TIM2_BASE + 0x0c)
#define TIM2_SR    (volatile uint32_t *)(TIM2_BASE + 0x10)
#define TIM2_EGR   (volatile uint32_t *)(TIM2_BASE + 0x14)
#define TIM2_CCMR1 (volatile uint32_t *)(TIM2_BASE + 0x18)
#define TIM2_CCMR2 (volatile uint32_t *)(TIM2_BASE + 0x1c)
#define TIM2_CCER  (volatile uint32_t *)(TIM2_BASE + 0x20)
#define TIM2_TCNT  (volatile uint32_t *)(TIM2_BASE + 0x24)
#define TIM2_PSC   (volatile uint32_t *)(TIM2_BASE + 0x28)
#define TIM2_ARR   (volatile uint32_t *)(TIM2_BASE + 0x2c)
#define TIM2_CCR1  (volatile uint32_t *)(TIM2_BASE + 0x34)
#define TIM2_CCR2  (volatile uint32_t *)(TIM2_BASE + 0x38)
#define TIM2_CCR3  (volatile uint32_t *)(TIM2_BASE + 0x3c)
#define TIM2_CCR4  (volatile uint32_t *)(TIM2_BASE + 0x40)
#define TIM2_DCR   (volatile uint32_t *)(TIM2_BASE + 0x48)
#define TIM2_DMAR  (volatile uint32_t *)(TIM2_BASE + 0x4c)

#define TIMx_CEN  (1<<0)
#define TIMx_UDIS (1<<1)
#define TIMx_URS  (1<<2)
#define TIMx_OPM  (1<<3)
#define TIMx_DIR  (1<<4)
#define TIMx_CMS  (3<<5)
#define TIMx_ARPE (1<<7)
#define TIMx_CKD  (3<<8)

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
  tim2_setup(true, CALC_PSC(0.1, F_CLK, 0xffff), 0xffff, true, true);
}

uint16_t tim2_read_counter_value(void)
{
  return reg_read(TIM2_TCNT);
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

