#include "gpio.h"
#include "memory_layout.h"
#include "reg_access.h"
#include "svc.h"

#define GPIO_WORDS_PER_PORT ((IOPB_BASE - IOPA_BASE)/4)
#define GPIO_PORT_CNT ((IOPG_BASE - IOPA_BASE) / GPIO_WORDS_PER_PORT)

#define AFIO_EVCR (volatile uint32_t *)(AFIO_BASE + 0x00)
#define AFIO_MAPR (volatile uint32_t *)(AFIO_BASE + 0x04)
#define AFIO_EXTICR1 (volatile uint32_t *)(AFIO_BASE + 0x08)
#define AFIO_EXTICR2 (volatile uint32_t *)(AFIO_BASE + 0x0c)
#define AFIO_EXTICR3 (volatile uint32_t *)(AFIO_BASE + 0x10)
#define AFIO_EXTICR4 (volatile uint32_t *)(AFIO_BASE + 0x14)
#define AFIO_MAPR2 (volatile uint32_t *)(AFIO_BASE + 0x18)

#define GPIOA_CRL  (volatile uint32_t *)(IOPA_BASE + 0x00)
#define GPIOA_CRH  (volatile uint32_t *)(IOPA_BASE + 0x04)
#define GPIOA_IDR  (volatile uint32_t *)(IOPA_BASE + 0x08)
#define GPIOA_ODR  (volatile uint32_t *)(IOPA_BASE + 0x0c)
#define GPIOA_BSRR (volatile uint32_t *)(IOPA_BASE + 0x10)
#define GPIOA_BRR  (volatile uint32_t *)(IOPA_BASE + 0x14)
#define GPIOA_LCKR (volatile uint32_t *)(IOPA_BASE + 0x14)

#define GPIOB_CRL  (volatile uint32_t *)(IOPB_BASE + 0x00)
#define GPIOB_CRH  (volatile uint32_t *)(IOPB_BASE + 0x04)
#define GPIOB_IDR  (volatile uint32_t *)(IOPB_BASE + 0x08)
#define GPIOB_ODR  (volatile uint32_t *)(IOPB_BASE + 0x0c)
#define GPIOB_BSRR (volatile uint32_t *)(IOPB_BASE + 0x10)
#define GPIOB_BRR  (volatile uint32_t *)(IOPB_BASE + 0x14)
#define GPIOB_LCKR (volatile uint32_t *)(IOPB_BASE + 0x14)

#define GPIOC_CRL  (volatile uint32_t *)(IOPC_BASE + 0x00)
#define GPIOC_CRH  (volatile uint32_t *)(IOPC_BASE + 0x04)
#define GPIOC_IDR  (volatile uint32_t *)(IOPC_BASE + 0x08)
#define GPIOC_ODR  (volatile uint32_t *)(IOPC_BASE + 0x0c)
#define GPIOC_BSRR (volatile uint32_t *)(IOPC_BASE + 0x10)
#define GPIOC_BRR  (volatile uint32_t *)(IOPC_BASE + 0x14)
#define GPIOC_LCKR (volatile uint32_t *)(IOPC_BASE + 0x14)

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

void gpio_setup(int port, int pin, int mode, int cnf)
{
  volatile uint32_t *r = GPIOA_CRL;

  if (port > GPIO_PORT_CNT)
    svc_call(SVC_PANIC);

  r += GPIO_WORDS_PER_PORT * port;

  gpiox_set_cr(r, pin, mode, cnf);
}


void gpiob_set_cr(int pin_nr, int mode, int cnf)
{
  gpiox_set_cr(GPIOB_CRL, pin_nr, mode, cnf);
}

void gpioc_set_pin13(void)
{
  gpioc_set_cr(13, 1, 1);
  gpioc_bit_set(13);
  gpioc_bit_clear(13);
}

void gpio_remap_i2c1(int mapping)
{
  reg32_modify_bit(AFIO_MAPR, 1, mapping);
}

bool gpiob_pin_is_set(int pin_nr)
{
  return reg32_bit_is_set(GPIOB_IDR, pin_nr);
}

void gpio_map_to_exti(int port, int pin_nr)
{
  volatile uint32_t *r;
  unsigned int exticr_reg_idx = pin_nr / 4;
  int exticr_pin_idx = pin_nr % 4;

  if (exticr_reg_idx >= 4) {
    svc_call(SVC_PANIC);
  }

  r =  AFIO_EXTICR1 + exticr_reg_idx;
  reg32_modify_bits(r, exticr_pin_idx * 4, 4, port);
}

void gpio_odr_modify(int port, int pin_nr, int set_clear)
{
  volatile uint32_t *r = GPIOA_ODR;

  if (port > GPIO_PORT_CNT)
    svc_call(SVC_PANIC);

  r += GPIO_WORDS_PER_PORT * port;
  reg32_modify_bits(r, pin_nr, 1, set_clear);
}
