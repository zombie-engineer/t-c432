#include "tim.h"
#include "reg_access.h"
#include "memory_layout.h"
#include "rcc.h"
#include "nvic.h"
#include "config.h"
#include "dma.h"

#define TIM2_CR1   (reg16_t)(TIM2_BASE + 0x00)
#define TIM2_CR2   (reg16_t)(TIM2_BASE + 0x04)
#define TIM2_SMCR  (reg16_t)(TIM2_BASE + 0x08)
#define TIM2_DIER  (reg16_t)(TIM2_BASE + 0x0c)
#define TIM2_SR    (reg16_t)(TIM2_BASE + 0x10)
#define TIM2_EGR   (reg16_t)(TIM2_BASE + 0x14)
#define TIM2_CCMR1 (reg16_t)(TIM2_BASE + 0x18)
#define TIM2_CCMR2 (reg16_t)(TIM2_BASE + 0x1c)
#define TIM2_CCER  (reg16_t)(TIM2_BASE + 0x20)
#define TIM2_CNT   (reg16_t)(TIM2_BASE + 0x24)
#define TIM2_PSC   (reg16_t)(TIM2_BASE + 0x28)
#define TIM2_ARR   (reg16_t)(TIM2_BASE + 0x2c)
#define TIM2_CCR1  (reg16_t)(TIM2_BASE + 0x34)
#define TIM2_CCR2  (reg16_t)(TIM2_BASE + 0x38)
#define TIM2_CCR3  (reg16_t)(TIM2_BASE + 0x3c)
#define TIM2_CCR4  (reg16_t)(TIM2_BASE + 0x40)
#define TIM2_DCR   (reg16_t)(TIM2_BASE + 0x48)
#define TIM2_DMAR  (reg16_t)(TIM2_BASE + 0x4c)

#define TIM3_CR1   (reg16_t)(TIM3_BASE + 0x00)
#define TIM3_CR2   (reg16_t)(TIM3_BASE + 0x04)
#define TIM3_SMCR  (reg16_t)(TIM3_BASE + 0x08)
#define TIM3_DIER  (reg16_t)(TIM3_BASE + 0x0c)
#define TIM3_SR    (reg16_t)(TIM3_BASE + 0x10)
#define TIM3_EGR   (reg16_t)(TIM3_BASE + 0x14)
#define TIM3_CCMR1 (reg16_t)(TIM3_BASE + 0x18)
#define TIM3_CCMR2 (reg16_t)(TIM3_BASE + 0x1c)

#define TIMx_CCMRx_OCxM 4
#define TIMx_CCMRx_OCxM_WIDTH 3

#define TIMx_CCMR1_CC1S 0
#define TIMx_CCMR1_CC1S_WIDTH 2

#define TIMx_CCMR1_CCxS_OUTPUT 0
#define TIMx_CCMR1_CCxS_IN_TI2 1
#define TIMx_CCMR1_CCxS_IN_TI1 2
#define TIMx_CCMR1_CCxS_IN_TRC 3

#define TIMx_CCMR1_CC2S 8
#define TIMx_CCMR1_CC2S_WIDTH 2

#define TIMx_CCMR1_OC1M TIMx_CCMRx_OCxM
#define TIMx_CCMR1_OC1M_WIDTH TIMx_CCMRx_OCxM_WIDTH
#define TIMx_CCMR1_OC2M (TIMx_CCMR1_OC1M + 8)
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

#define TIMx_SMCR_SMS 0
#define TIMx_SMCR_SMS_WIDTH 3
#define TIMx_SMCR_SMS_DISA 0
#define TIMx_SMCR_SMS_ENCODER_1 1
#define TIMx_SMCR_SMS_ENCODER_2 2
#define TIMx_SMCR_SMS_ENCODER_3 3
#define TIMx_SMCR_SMS_RESET 4
#define TIMx_SMCR_SMS_GATED 5
#define TIMx_SMCR_SMS_TRIGGER 6
#define TIMx_SMCR_SMS_EXT_CLK_1 7

#define TIMx_SMCR_TS 4
#define TIMx_SMCR_TS_WIDTH 3
#define TIMx_SMCR_TS_ITR0 0
#define TIMx_SMCR_TS_ITR1 1
#define TIMx_SMCR_TS_ITR2 2
#define TIMx_SMCR_TS_ITR3 3
#define TIMx_SMCR_TS_TI1 4
#define TIMx_SMCR_TS_TI1FP1 5
#define TIMx_SMCR_TS_TI2FP2 6
#define TIMx_SMCR_TS_ETRF 7

#define TIM3_CCER  (reg16_t)(TIM3_BASE + 0x20)
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

#define TIM3_CNT   (reg16_t)(TIM3_BASE + 0x24)
#define TIM3_PSC   (reg16_t)(TIM3_BASE + 0x28)
#define TIM3_ARR   (reg16_t)(TIM3_BASE + 0x2c)

#define TIM3_CCR1  (reg16_t)(TIM3_BASE + 0x34)
#define TIM3_CCR2  (reg16_t)(TIM3_BASE + 0x38)
#define TIM3_CCR3  (reg16_t)(TIM3_BASE + 0x3c)
#define TIM3_CCR4  (reg16_t)(TIM3_BASE + 0x40)

#define TIM3_DCR   (reg16_t)(TIM3_BASE + 0x48)
#define TIM3_DMAR  (reg16_t)(TIM3_BASE + 0x4c)

#define TIMx_CR1_CEN  (1<<0)
#define TIMx_CR1_UDIS (1<<1)
#define TIMx_CR1_URS  (1<<2)
#define TIMx_CR1_OPM  (1<<3)
#define TIMx_CR1_DIR  (1<<4)
#define TIMx_CR1_CMS  (3<<5)
#define TIMx_CR1_ARPE (1<<7)
#define TIMx_CR1_CKD  (3<<8)

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

int tim3_pwm_dma_setup(tim3_channel_id ch)
{
  reg16_t ccr;
  reg16_t ccmr;
  int ccmr_ocm_offset;
  int ccmr_ocpe_offset;

  if (ch > TIM3_CH_MAX)
    return -1;

  ccr = TIM3_CCR1 + ch * 2;
  ccmr = TIM3_CCMR1 + ch;
  ccmr_ocm_offset = 4 + (ch % 2) * 8;
  ccmr_ocpe_offset = 3 + (ch % 2) * 8;

  reg_write16(ccr, 0);

  reg_write16(ccmr,
    (TIMx_CCMRx_OCxM_PWM1 << ccmr_ocm_offset)
    | (TIMx_CCRMx_OCxPE_PRELOAD << ccmr_ocpe_offset));

  reg_write16(TIM3_DIER, 1 << TIMx_DIER_UDE);
  reg_write16(TIM3_DCR, (ccr - TIM3_CR1) & 0x1f);
}

void tim3_setup(uint16_t prescaler, uint16_t period)
{
  uint32_t v;
  rcc_periph_ena(RCC_PERIPH_TIM3);
  reg_write16(TIM3_CNT, 0);
  reg_write16(TIM3_PSC, prescaler);
  reg_write16(TIM3_ARR, period);


  reg_write16(TIM3_CCER, TIMx_CCER_CCxE_ON << TIMx_CCER_CC3E);

  reg_write16(TIM3_CR1, TIMx_CR1_ARPE);
}

void tim3_enable(void)
{
  reg16_set_bit(TIM3_CR1, TIMx_CR1_CEN);
}

void tim3_disable(void)
{
  reg_write16(TIM3_CR1, 0);
}

void tim3_pwm_dma_run(int dma_channel, const void *src, int src_size)
{
  reg_write16(TIM3_CNT, 0);
  reg_write16(TIM3_CCR3, 0);
  reg_write16(TIM3_CR1, TIMx_CR1_CEN | TIMx_CR1_ARPE);
  struct dma_channel_settings dma_settings = {
    .paddr = TIM3_CCR3,
    .maddr = (void *)src,
    .count = src_size,
    .dir = DMA_TRANSFER_DIR_TO_PERIPH,
    .pwidth = 16,
    .mwidth = 8,
    .circular = false,
    .pinc = false,
    .minc = true,
    .interrupt_on_completion = true,
    .enable_after_setup = true
  };

  dma_transfer_setup(dma_channel, &dma_settings);
}

void tim2_setup_pulse_counting(void)
{
  uint32_t reg = 0;

  u32_modify_bits(&reg, TIMx_CCMR1_CC2S, TIMx_CCMR1_CC2S_WIDTH,
    TIMx_CCMR1_CCxS_IN_TI2);

  reg_write(TIM2_CCMR1, reg);

  reg = 0;
  u32_modify_bits(&reg, TIMx_SMCR_SMS, TIMx_SMCR_SMS_WIDTH,
    TIMx_SMCR_SMS_EXT_CLK_1);

  u32_modify_bits(&reg, TIMx_SMCR_SMS, TIMx_SMCR_SMS_WIDTH,
    TIMx_SMCR_TS_TI2FP2);

  reg_write(TIM2_SMCR, reg);
  reg_write(TIM2_ARR, 0xffff);
  reg_write(TIM2_PSC, 0);

  reg = 0;
  u32_set_bit(&reg, TIMx_CR1_CEN);
  reg_write(TIM2_CR1, reg);
}

int tim2_setup(bool one_pulse, uint16_t prescaler, uint16_t counter_value,
  bool enable_interrupt, bool enable)
{
  uint16_t v;

  reg_write16(TIM2_ARR, counter_value);
  reg_write16(TIM2_PSC, prescaler);

  v = reg_read16(TIM2_CR1);

  /* Auto-reload */
  v |= TIMx_CR1_ARPE;

  /* center-aligned off */
  v &= ~TIMx_CR1_CMS;

  /* count dir = up */
  v &= ~TIMx_CR1_DIR;

  if (one_pulse)
    v |= TIMx_CR1_OPM;
  else
    v &= ~TIMx_CR1_OPM;

  /* Update only from overflow */
  v |= TIMx_CR1_URS;

  /* Enable interrupt for TIM2 */
  if (enable_interrupt) {
    /* Event on overflow enabled */
    reg_write16(TIM2_SR, 0);
    reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
    v &= TIMx_CR1_UDIS;
    reg_write(TIM2_DIER, 1);
    reg_write(NVIC_ISER0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
  }

  if (enable) {
    v |= TIMx_CR1_CEN;;
  }

  reg_write16(TIM2_CR1, v);
  
  return 0;
}

uint16_t tim2_read_counter_value(void)
{
  return reg_read16(TIM2_CNT);
}

void __attribute__((weak)) tim2_isr_cb(void)
{
}

void __tim2_isr(void)
{
  reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
  reg_write16(TIM2_SR, 0);
  reg16_set_bit(TIM2_CR1, 0);
  tim2_isr_cb();
}
