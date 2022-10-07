#include "nvic.h"
#include "reg_access.h"

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
