#include "nvic.h"
#include "reg_access.h"
#include "svc.h"

uint8_t nvic_get_priority(int interrupt_no)
{
  volatile uint8_t *reg = ((volatile uint8_t *)NVIC_IPR0) + interrupt_no;
  return ((*reg) >> 4) & 0xf;
}

void nvic_set_priority(int interrupt_no, uint8_t pri)
{
  volatile uint8_t *reg = ((volatile uint8_t *)NVIC_IPR0) + interrupt_no;
  *reg = (pri & 0xf) << 4;
}

void nvic_enable_interrupt(int interrupt_no)
{
  volatile uint32_t *r;

  if (interrupt_no > NVIC_INTERRUPT_NUMBER_MAX) {
    svc_call(SVC_PANIC);
  }

  r = NVIC_ISER0 + interrupt_no / NVIC_ICER_INTS_PER_REG;
  reg32_set_bit(r, interrupt_no % NVIC_ICER_INTS_PER_REG);
}
