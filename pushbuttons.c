#include "pushbuttons.h"
#include "gpio.h"
#include "exti.h"
#include "nvic.h"
#include "common_util.h"
#include "config.h"

#define BUTTON_STATE_PRESSED 0
#define BUTTON_STATE_RELEASED 1

struct button_info {
  uint8_t pin;
  uint8_t port;
  uint8_t state;
  exti_interrupt_callback cb;
};

struct button_info buttons[3] = { 0 };

extern void ui_callback_button_event_pressed(int button_id);
extern void ui_callback_button_event_released(int button_id);

static void pushbutton_signal(int button_idx)
{
  struct button_info *b;
  if (button_idx >= ARRAY_SIZE(buttons))
    return;

  b = &buttons[button_idx];

  if (gpio_pin_is_set(b->port, b->pin)) {
    if (b->state == BUTTON_STATE_RELEASED) {
      ui_callback_button_event_pressed(button_idx);
    }
    b->state = BUTTON_STATE_PRESSED;
  } else {
    if (b->state == BUTTON_STATE_PRESSED) {
      ui_callback_button_event_released(button_idx);
    }
    b->state = BUTTON_STATE_RELEASED;
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

static void pushbutton_signal_c(void)
{
  pushbutton_signal(2);
}

void pushbutton_init_single(struct button_info *b)
{
  gpio_setup(b->port, b->pin, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpio_odr_modify(b->port, b->pin, 0);
  exti_enable_gpio_interrupt(b->pin,
    (1<<EXTI_TRIGGER_FLAG_RISING) | (1<<EXTI_TRIGGER_FLAG_FALLING));
  exti_register_callback(b->pin, b->cb);
  gpio_map_to_exti(b->port, b->pin);

  if (b->pin < 5)
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI0 + b->pin);
  else if (b->pin < 10)
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI9_5);
  else if (b->pin < 16)
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI15_10);
}

void pushbuttons_init(void)
{
  buttons[0].pin = CNF_BUTTON_KEY_LEFT_PIN;
  buttons[0].port = GPIO_PORT(CNF_BUTTON_KEY_LEFT_PORT);
  buttons[0].state = BUTTON_STATE_RELEASED;
  buttons[0].cb = pushbutton_signal_a;
  buttons[1].pin = CNF_BUTTON_KEY_RIGHT_PIN;
  buttons[1].port = GPIO_PORT(CNF_BUTTON_KEY_RIGHT_PORT);
  buttons[1].state = BUTTON_STATE_RELEASED;
  buttons[1].cb = pushbutton_signal_b;
  buttons[2].pin = CNF_BUTTON_KEY_MID_PIN;
  buttons[2].port = GPIO_PORT(CNF_BUTTON_KEY_MID_PORT);
  buttons[2].state = BUTTON_STATE_RELEASED;
  buttons[2].cb = pushbutton_signal_c;

  for (int i = 0; i < 3; ++i) {
    pushbutton_init_single(&buttons[i]);
  }
}

bool pushbotton_a_is_pressed(void)
{
  return buttons[0].state == BUTTON_STATE_PRESSED;
}

bool pushbotton_b_is_pressed(void)
{
  return buttons[1].state == BUTTON_STATE_PRESSED;
}

bool pushbotton_c_is_pressed(void)
{
  return false;
}

