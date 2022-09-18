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

#define THUMB __attribute__((target("thumb")))
static THUMB void rcc_cr_enable_hse(void)
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

static THUMB void rcc_cfgr_select_hse(void)
{
  uint32_t v;
  v = reg_read(RCC_CFGR);
  v &= ~RCC_CFGR_SW_MASK;
  v |= RCC_CFGR_SW_HSE;
  reg_write(RCC_CFGR, v);
  while ((reg_read(RCC_CFGR) & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_HSE);
}

static THUMB void rcc_apb1rsr_reset_tim2(void)
{
  uint32_t v;
  v = reg_read(RCC_APB1RSTR);
  v |= RCC_APB1RSTR_TIM2RST;
  reg_write(RCC_APB1RSTR, v);
}

static THUMB void rcc_apb1enr_enable_tim2(void)
{
  uint32_t v;
  v = reg_read(RCC_APB1ENR);
  v |= RCC_APB1ENR_TIM2EN;
  reg_write(RCC_APB1ENR, v);
}

static THUMB void rcc_apb2rsr_reset_iopc(void)
{
  uint32_t v;
  v = reg_read(RCC_APB2RSTR);
  v |= RCC_APB1RSTR_IOPCRST;
  reg_write(RCC_APB2RSTR, v);
}

static THUMB void rcc_apb2enr_enable_iopc(void)
{
  uint32_t v;
  v = reg_read(RCC_APB2ENR);
  v |= RCC_APB1ENR_IOPCEN;
  reg_write(RCC_APB2ENR, v);
}

static THUMB void gpioc_set_pin13(void)
{
  uint32_t v;
  v = reg_read(GPIOC_CRH);
  v &= ~(3<<20);
  v |= (1<<20);
  v &= ~(3<<22);
  v |= (1<<22);
  reg_write(GPIOC_CRH, v);
  while(1);
}

THUMB void main(void)
{
#if 0
  rcc_cr_enable_hse();
  rcc_cfgr_select_hse();
  rcc_apb1rsr_reset_tim2();
  rcc_apb1enr_enable_tim2();
#endif

 // rcc_apb2rsr_reset_iopc();
  rcc_apb2enr_enable_iopc();
  gpioc_set_pin13();
}
