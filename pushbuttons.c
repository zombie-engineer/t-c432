#include "pushbuttons.h"
#include "gpio.h"
#include "exti.h"
#include "nvic.h"
#include "common_util.h"

#define BUTTON_STATE_PRESSED 0
#define BUTTON_STATE_RELEASED 1
struct button_state {
  int pin_nr;
  int state;
};

struct button_state buttons[3] = { 0 };

static void pushbutton_signal(int button_idx)
{
  struct button_state *b;
  if (button_idx >= ARRAY_SIZE(buttons))
    return;

  b = &buttons[button_idx];
  /*
   * Previosly released, now became pressed, so now we should wait for
   * falling edge event for release event
   */
  if (b->state == BUTTON_STATE_RELEASED) {
    b->state = BUTTON_STATE_PRESSED;
    exti_enable_gpio_interrupt(b->pin_nr, 1<<EXTI_TRIGGER_FLAG_FALLING);
  } else {
    b->state = BUTTON_STATE_RELEASED;
    exti_enable_gpio_interrupt(b->pin_nr, 1<<EXTI_TRIGGER_FLAG_RISING);
  }
}

static void pushbutton_signal_a(void)
{
  pushbutton_signal(0);
}

static void pushbutton_signal_b(void)
{
  pushbutton_signal(1);
}

void pushbuttons_init(void)
{
  buttons[0].pin_nr = 8;
  buttons[0].state = BUTTON_STATE_RELEASED;
  buttons[1].pin_nr = 9;
  buttons[2].state = BUTTON_STATE_RELEASED;
  gpiob_set_cr(8, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpiob_set_cr(9, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpio_odr_modify(GPIO_PORT_B, 8, 0);
  gpio_odr_modify(GPIO_PORT_B, 9, 0);
  exti_enable_gpio_interrupt(8, 1<<EXTI_TRIGGER_FLAG_RISING);
  exti_enable_gpio_interrupt(9, 1<<EXTI_TRIGGER_FLAG_RISING);
  exti_register_callback(PUSHBUTTON_A, pushbutton_signal_a);
  exti_register_callback(PUSHBUTTON_B, pushbutton_signal_b);
  gpio_map_to_exti(GPIO_PORT_B, 8);
  gpio_map_to_exti(GPIO_PORT_B, 9);
  nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI9_5);
}

bool pushbotton_a_is_pressed(void)
{
  return buttons[0].state == BUTTON_STATE_PRESSED;
  //return gpiob_pin_is_set(8);
}

bool pushbotton_b_is_pressed(void)
{
  return buttons[1].state == BUTTON_STATE_PRESSED;
  //return gpiob_pin_is_set(9);
}

bool pushbotton_c_is_pressed(void)
{
  return false;
}

