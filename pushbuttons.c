#include "pushbuttons.h"
#include "gpio.h"
#include "exti.h"
#include "nvic.h"

void pushbuttons_init(void)
{
  gpiob_set_cr(8, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpiob_set_cr(9, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  exti_enable_gpio_interrupt(8);
  exti_enable_gpio_interrupt(9);
  gpio_map_to_exti(GPIO_PORT_B, 8);
  gpio_map_to_exti(GPIO_PORT_B, 9);
  nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI9_5);
}

bool pushbotton_a_is_pressed(void)
{
  gpiob_pin_is_set(8);
}

bool pushbotton_b_is_pressed(void)
{
  gpiob_pin_is_set(9);
}

bool pushbotton_c_is_pressed(void)
{
}
