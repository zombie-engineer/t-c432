#include <stdint.h>
#define RCC_BASE 0x40021000
#define RCC_CR       (RCC_BASE + 0x00)
#define RCC_CFGR     (RCC_BASE + 0x04)
#define RCC_APB2RSTR (RCC_BASE + 0x0c)
#define RCC_APB1RSTR (RCC_BASE + 0x10)
#define RCC_AHBENR   (RCC_BASE + 0x14)
#define RCC_APB2ENR  (RCC_BASE + 0x18)
#define RCC_APB1ENR  (RCC_BASE + 0x1c)

#define reg_write(__reg, __value) \
  *(volatile uint32_t *)(__reg) = (__value)

#define reg_read(__reg) \
  (*(volatile uint32_t *)(__reg))

#define RCC_CFGR_SW_HSI  0b00
#define RCC_CFGR_SW_HSE  0b01
#define RCC_CFGR_SW_PLL  0b10
#define RCC_CFGR_SW_MASK 0b11

#define RCC_CFGR_SWS_HSI  0b0000
#define RCC_CFGR_SWS_HSE  0b0100
#define RCC_CFGR_SWS_PLL  0b1000
#define RCC_CFGR_SWS_MASK 0b1100

#define RCC_CR_HSEON_POS 16
#define RCC_CR_HSERDY_POS 17

#define RCC_APB1RSTR_TIM2RST 1
#define RCC_APB1ENR_TIM2EN 1
#define RCC_APB1RSTR_IOPCRST (1<<4)
#define RCC_APB1ENR_IOPCEN (1<<4)

#define IOPC_BASE 0x40011000
#define GPIOC_CRL  (IOPC_BASE + 0x00)
#define GPIOC_CRH  (IOPC_BASE + 0x04)
#define GPIOC_IDR  (IOPC_BASE + 0x08)
#define GPIOC_ODR  (IOPC_BASE + 0x0c)
#define GPIOC_BSRR (IOPC_BASE + 0x10)
#define GPIOC_BRR  (IOPC_BASE + 0x14)
#define GPIOC_LCKR (IOPC_BASE + 0x14)

#define TIM2_BASE 0x40000000

#define TIM2_CR1   (TIM2_BASE + 0x00)
#define TIM2_CR2   (TIM2_BASE + 0x04)
#define TIM2_SMCR  (TIM2_BASE + 0x08)
#define TIM2_DIER  (TIM2_BASE + 0x0c)
#define TIM2_SR    (TIM2_BASE + 0x10)
#define TIM2_EGR   (TIM2_BASE + 0x14)
#define TIM2_CCMR1 (TIM2_BASE + 0x18)
#define TIM2_CCMR2 (TIM2_BASE + 0x1c)
#define TIM2_CCER  (TIM2_BASE + 0x20)
#define TIM2_TCNT  (TIM2_BASE + 0x24)
#define TIM2_TPSC  (TIM2_BASE + 0x28)
#define TIM2_ARR   (TIM2_BASE + 0x2c)
#define TIM2_CCR1  (TIM2_BASE + 0x34)
#define TIM2_CCR2  (TIM2_BASE + 0x38)
#define TIM2_CCR3  (TIM2_BASE + 0x3c)
#define TIM2_CCR4  (TIM2_BASE + 0x40)
#define TIM2_DCR   (TIM2_BASE + 0x48)
#define TIM2_DMAR  (TIM2_BASE + 0x4c)

#define THUMB __attribute__((target("thumb")))

static void rcc_cr_enable_hse(void)
{
  uint32_t v = reg_read(RCC_CR);
  if (v & (1 << RCC_CR_HSERDY_POS))
  {
    return;
  }
  v |= (1 << RCC_CR_HSEON_POS); 
  reg_write(RCC_CR, v);
  while(reg_read(RCC_CR) & (1 << RCC_CR_HSERDY_POS) == 0);
}

static void rcc_cfgr_select_hse(void)
{
  uint32_t v;
  v = reg_read(RCC_CFGR);
  v &= ~RCC_CFGR_SW_MASK;
  v |= RCC_CFGR_SW_HSE;
  reg_write(RCC_CFGR, v);
  while ((reg_read(RCC_CFGR) & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_HSE);
}

static void rcc_apb1rsr_reset_tim2(void)
{
  uint32_t v;
  v = reg_read(RCC_APB1RSTR);
  v |= RCC_APB1RSTR_TIM2RST;
  reg_write(RCC_APB1RSTR, v);
}

static void rcc_apb1enr_enable_tim2(void)
{
  uint32_t v;
  v = reg_read(RCC_APB1ENR);
  v |= RCC_APB1ENR_TIM2EN;
  reg_write(RCC_APB1ENR, v);
}

static void rcc_apb2rsr_reset_iopc(void)
{
  uint32_t v;
  v = reg_read(RCC_APB2RSTR);
  v |= RCC_APB1RSTR_IOPCRST;
  reg_write(RCC_APB2RSTR, v);
}

static void rcc_apb2enr_enable_iopc(void)
{
  uint32_t v;
  v = reg_read(RCC_APB2ENR);
  v |= RCC_APB1ENR_IOPCEN;
  reg_write(RCC_APB2ENR, v);
}

static void gpioc_set_pin13(void)
{
  uint32_t v;
  v = reg_read(GPIOC_CRH);
  v &= ~(3<<20);
  v |= (1<<20);
  v &= ~(3<<22);
  v |= (1<<22);
  reg_write(GPIOC_CRH, v);
}

static int tim2_setup(void)
{
  uint16_t v;

  reg_write(TIM2_ARR, 0xffff);

  v = reg_read(TIM2_CR1);

  // Prescaler / 4
  v &= ~0xfcff;
  v |= 2 << 8;

  // Auto-reload
  v |= 1 << 7;

  // Not center-aligned
  v &= 0xff9f;

  // Up count
  v &= 0xffef;

  // No one-pulse
  v &= 0xfff7;

  // Update only from overflow
  v |= 4;

  // UEV not sent
  v &= 0xfffd;

  // Enable counter
  v |= 1;
  reg_write(TIM2_CR1, v);

  return 0;
}

void tim2_handler(void)
{
}

void main(void)
{
#if 0
  rcc_cr_enable_hse();
  rcc_cfgr_select_hse();
  rcc_apb2rsr_reset_iopc();
#endif

  // rcc_apb1rsr_reset_tim2();
  rcc_apb1enr_enable_tim2();

  rcc_apb2enr_enable_iopc();
  gpioc_set_pin13();
  tim2_setup();
}
