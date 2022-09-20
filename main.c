#include <stdint.h>
#include <stdbool.h>

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
#define TIM2_PSC   (TIM2_BASE + 0x28)
#define TIM2_ARR   (TIM2_BASE + 0x2c)
#define TIM2_CCR1  (TIM2_BASE + 0x34)
#define TIM2_CCR2  (TIM2_BASE + 0x38)
#define TIM2_CCR3  (TIM2_BASE + 0x3c)
#define TIM2_CCR4  (TIM2_BASE + 0x40)
#define TIM2_DCR   (TIM2_BASE + 0x48)
#define TIM2_DMAR  (TIM2_BASE + 0x4c)

#define NVIC_BASE 0xe000e100
#define NVIC_ISER0 (NVIC_BASE + 0x00)
#define NVIC_ISER1 (NVIC_BASE + 0x04)
#define NVIC_ISER2 (NVIC_BASE + 0x08)

#define NVIC_ICER0 (NVIC_BASE + 0x80)
#define NVIC_ICER1 (NVIC_BASE + 0x84)
#define NVIC_ICER2 (NVIC_BASE + 0x88)

#define NVIC_ISPR0 (NVIC_BASE + 0x100)
#define NVIC_ISPR1 (NVIC_BASE + 0x104)
#define NVIC_ISPR2 (NVIC_BASE + 0x108)

#define NVIC_ICPR0 (NVIC_BASE + 0x180)
#define NVIC_ICPR1 (NVIC_BASE + 0x184)
#define NVIC_ICPR2 (NVIC_BASE + 0x188)

#define NVIC_IABR0 (NVIC_BASE + 0x200)
#define NVIC_IABR1 (NVIC_BASE + 0x204)
#define NVIC_IABR2 (NVIC_BASE + 0x208)

#define NVIC_IPR0 (NVIC_BASE + 0x300)
#define NVIC_STIR (NVIC_BASE + 0xe00)

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


static void clear_and_set_bitfield_32(uint32_t *v, int bitpos, int bitwidth, int value)
{
  uint32_t tmp_v = *v;
  uint32_t bitmask = ((uint32_t)((1 << bitwidth) - 1)) << bitpos;
  tmp_v &= ~bitmask;
  tmp_v |= (value << bitpos) & bitmask;
  *v = tmp_v;
}

static void gpioc_bit_set(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1<<pin_nr);
}

static void gpioc_bit_clear(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1 << (pin_nr + 16));
}

static void gpioc_set_cr(int pin_nr, int mode, int cnf)
{
  uint32_t v;
  int b;
  uint32_t addr;

  addr = GPIOC_CRL;
  addr += (pin_nr / 8) * 4;
  b = (pin_nr % 8) * 4;

  v = reg_read(addr);
  clear_and_set_bitfield_32(&v, b, 2, mode);
  clear_and_set_bitfield_32(&v, b + 2, 2, cnf);
  reg_write(addr, v);
}

static void gpioc_set_pin13(void)
{
  gpioc_set_cr(13, 1, 1);
  gpioc_bit_set(13);
  gpioc_bit_clear(13);
}

typedef enum {
  RCC_SRC_HSI,
  RCC_SRC_HSE,
  RCC_SRC_PLL,
} rcc_src_enum;

#define TIMx_CEN  (1<<0)
#define TIMx_UDIS (1<<1)
#define TIMx_URS  (1<<2)
#define TIMx_OPM  (1<<3)
#define TIMx_DIR  (1<<4)
#define TIMx_CMS  (3<<5)
#define TIMx_ARPE (1<<7)
#define TIMx_CKD  (3<<8)

#define NVIC_INTERRUPT_NUMBER_TIM2 28
static int tim2_setup(
  bool one_pulse,
  uint16_t prescaler,
  uint16_t counter_value,
  bool enable_interrupt,
  bool enable)
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

void tim2_isr(void)
{
  static int toggle_flag = 0;
  reg_write(NVIC_ICPR0, 1 << NVIC_INTERRUPT_NUMBER_TIM2);
  reg_write(TIM2_SR, 0);
  reg_write(TIM2_CR1, reg_read(TIM2_CR1) | 1);

  if (toggle_flag) {
    gpioc_bit_set(13);
    toggle_flag = 0;
  } else {
    gpioc_bit_clear(13);
    toggle_flag = 1;
  }
}

void main(void)
{
  rcc_apb1enr_enable_tim2();
  rcc_apb2enr_enable_iopc();
  gpioc_set_pin13();
  tim2_setup(true, 2, 0xffff, true, true);
  while(1);
}
