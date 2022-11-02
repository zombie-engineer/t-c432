#pragma once
#include <stdint.h>

#define EXTI_TRIGGER_FLAG_RISING 0
#define EXTI_TRIGGER_FLAG_FALLING 1
#define EXT_INTERRUPT_COUNT 16

void exti_enable_gpio_interrupt(int ext_int_nr, uint32_t trigger_flags);

typedef void (*exti_interrupt_callback)(void);

void exti_register_callback(int ext_int_nr, exti_interrupt_callback cb);
