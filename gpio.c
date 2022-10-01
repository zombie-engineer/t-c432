#include "gpio.h"
#include "memory_layout.h"
#include "reg_access.h"

#define GPIOC_CRL  (volatile uint32_t *)(IOPC_BASE + 0x00)
#define GPIOC_CRH  (volatile uint32_t *)(IOPC_BASE + 0x04)
#define GPIOC_IDR  (volatile uint32_t *)(IOPC_BASE + 0x08)
#define GPIOC_ODR  (volatile uint32_t *)(IOPC_BASE + 0x0c)
#define GPIOC_BSRR (volatile uint32_t *)(IOPC_BASE + 0x10)
#define GPIOC_BRR  (volatile uint32_t *)(IOPC_BASE + 0x14)
#define GPIOC_LCKR (volatile uint32_t *)(IOPC_BASE + 0x14)

#define GPIOA_CRL  (volatile uint32_t *)(IOPA_BASE + 0x00)
#define GPIOA_CRH  (volatile uint32_t *)(IOPA_BASE + 0x04)
#define GPIOA_IDR  (volatile uint32_t *)(IOPA_BASE + 0x08)
#define GPIOA_ODR  (volatile uint32_t *)(IOPA_BASE + 0x0c)
#define GPIOA_BSRR (volatile uint32_t *)(IOPA_BASE + 0x10)
#define GPIOA_BRR  (volatile uint32_t *)(IOPA_BASE + 0x14)
#define GPIOA_LCKR (volatile uint32_t *)(IOPA_BASE + 0x14)

void gpioc_bit_set(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1<<pin_nr);
}

void gpioc_bit_clear(int pin_nr)
{
  reg_write(GPIOC_BSRR, 1 << (pin_nr + 16));
}

static void gpiox_set_cr(volatile uint32_t *basereg, int pin_nr, int mode, int cnf)
{
  uint32_t v;
  int b;
  volatile uint32_t *addr;

  addr = basereg;
  addr += (pin_nr / 8) * (4/4);
  b = (pin_nr % 8) * 4;

  v = reg_read(addr);
  u32_modify_bits(&v, b, 2, mode);
  u32_modify_bits(&v, b + 2, 2, cnf);
  reg_write(addr, v);
}

static void gpioc_set_cr(int pin_nr, int mode, int cnf)
{
  gpiox_set_cr(GPIOC_CRL, pin_nr, mode, cnf);
}

void gpioa_set_cr(int pin_nr, int mode, int cnf)
{
  gpiox_set_cr(GPIOA_CRL, pin_nr, mode, cnf);
}

void gpioc_set_pin13(void)
{
  gpioc_set_cr(13, 1, 1);
  gpioc_bit_set(13);
  gpioc_bit_clear(13);
}

void gpioa_set_odr(int bit)
{
  reg32_set_bit(GPIOA_ODR, bit);
}
