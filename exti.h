#pragma once
#include <stdint.h>

#define EXTI_TRIGGER_FLAG_RISING 0
#define EXTI_TRIGGER_FLAG_FALLING 1
void exti_enable_gpio_interrupt(int pin_nr, uint32_t trigger_flags);
