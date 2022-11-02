#include "exti.h"
#include "memory_layout.h"
#include "reg_access.h"

#define EXTI_IMR (volatile uint32_t *)(EXTI_BASE + 0x00)
#define EXTI_EMR (volatile uint32_t *)(EXTI_BASE + 0x04)
#define EXTI_RSTR (volatile uint32_t *)(EXTI_BASE + 0x08)
#define EXTI_FSTR (volatile uint32_t *)(EXTI_BASE + 0x0c)
#define EXTI_SWIER (volatile uint32_t *)(EXTI_BASE + 0x10)
#define EXTI_PR (volatile uint32_t *)(EXTI_BASE + 0x14)

void exti_enable_gpio_interrupt(int pin_nr, uint32_t trigger_flags)
{
  reg32_set_bit(EXTI_IMR, pin_nr);
  if (u32_bit_is_set(trigger_flags, EXTI_TRIGGER_FLAG_RISING))
    reg32_set_bit(EXTI_RSTR, pin_nr);
  if (u32_bit_is_set(trigger_flags, EXTI_TRIGGER_FLAG_FALLING))
    reg32_set_bit(EXTI_FSTR, pin_nr);
}

void exti_isr(int ext_interrupt_no)
{
  asm volatile("bkpt");
}
