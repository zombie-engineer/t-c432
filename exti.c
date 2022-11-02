#include "exti.h"
#include "memory_layout.h"
#include "reg_access.h"

#define EXTI_IMR (volatile uint32_t *)(EXTI_BASE + 0x00)
#define EXTI_EMR (volatile uint32_t *)(EXTI_BASE + 0x04)
#define EXTI_RSTR (volatile uint32_t *)(EXTI_BASE + 0x08)
#define EXTI_FSTR (volatile uint32_t *)(EXTI_BASE + 0x0c)
#define EXTI_SWIER (volatile uint32_t *)(EXTI_BASE + 0x10)
#define EXTI_PR (volatile uint32_t *)(EXTI_BASE + 0x14)


static exti_interrupt_callback exti_callbacks[EXT_INTERRUPT_COUNT] = { 0 };

void exti_enable_gpio_interrupt(int pin_nr, uint32_t trigger_flags)
{
  reg32_set_bit(EXTI_IMR, pin_nr);
  if (u32_bit_is_set(trigger_flags, EXTI_TRIGGER_FLAG_RISING))
    reg32_set_bit(EXTI_RSTR, pin_nr);
  else
    reg32_clear_bit(EXTI_RSTR, pin_nr);
  if (u32_bit_is_set(trigger_flags, EXTI_TRIGGER_FLAG_FALLING))
    reg32_set_bit(EXTI_FSTR, pin_nr);
  else
    reg32_clear_bit(EXTI_FSTR, pin_nr);
}

void exti_isr(int ext_int_nr)
{
  if (ext_int_nr >= EXT_INTERRUPT_COUNT)
    return;

  reg32_write_clear_bit(EXTI_PR, ext_int_nr);
  if (exti_callbacks[ext_int_nr])
    exti_callbacks[ext_int_nr]();
}

void exti_register_callback(int ext_int_nr, exti_interrupt_callback cb)
{
  if (ext_int_nr >= EXT_INTERRUPT_COUNT)
    return;

  exti_callbacks[ext_int_nr] = cb;
}
