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

struct button_info buttons[PUSHBUTTON_ID_COUNT] = { 0 };

extern void ui_callback_button_event_pressed(int button_id);
extern void ui_callback_button_event_released(int button_id);

static void pushbutton_signal(int button_idx)
{
  struct button_info *b;
  if (button_idx >= ARRAY_SIZE(buttons))
    return;

  b = &buttons[button_idx];

  if (!gpio_pin_is_set(b->port, b->pin)) {
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

static void pushbutton_int_left(void)
{
  pushbutton_signal(PUSHBUTTON_ID_LEFT);
}

static void pushbutton_int_right(void)
{
  pushbutton_signal(PUSHBUTTON_ID_RIGHT);
}

static void pushbutton_int_mid(void)
{
  pushbutton_signal(PUSHBUTTON_ID_MID);
}


void pushbutton_init_single(struct button_info *b)
{
  gpio_setup(b->port, b->pin, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  gpio_odr_modify(b->port, b->pin, 0);
  exti_register_callback(b->pin, b->cb);

  exti_clear_interrupts();
  gpio_map_to_exti(b->port, b->pin);
  exti_enable_gpio_interrupt(b->pin,
    (1<<EXTI_TRIGGER_FLAG_RISING) | (1<<EXTI_TRIGGER_FLAG_FALLING));

  if (b->pin < 5)
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI0 + b->pin);
  else if (b->pin < 10)
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI9_5);
  else if (b->pin < 16)
    nvic_enable_interrupt(NVIC_INTERRUPT_NUMBER_EXTI15_10);
}

void pushbuttons_init(void)
{
  buttons[PUSHBUTTON_ID_LEFT].pin = CNF_BUTTON_KEY_LEFT_PIN;
  buttons[PUSHBUTTON_ID_LEFT].port = GPIO_PORT(CNF_BUTTON_KEY_LEFT_PORT);
  buttons[PUSHBUTTON_ID_LEFT].state = BUTTON_STATE_RELEASED;
  buttons[PUSHBUTTON_ID_LEFT].cb = pushbutton_int_left;

  buttons[PUSHBUTTON_ID_RIGHT].pin = CNF_BUTTON_KEY_RIGHT_PIN;
  buttons[PUSHBUTTON_ID_RIGHT].port = GPIO_PORT(CNF_BUTTON_KEY_RIGHT_PORT);
  buttons[PUSHBUTTON_ID_RIGHT].state = BUTTON_STATE_RELEASED;
  buttons[PUSHBUTTON_ID_RIGHT].cb = pushbutton_int_right;

  buttons[PUSHBUTTON_ID_MID].pin = CNF_BUTTON_KEY_MID_PIN;
  buttons[PUSHBUTTON_ID_MID].port = GPIO_PORT(CNF_BUTTON_KEY_MID_PORT);
  buttons[PUSHBUTTON_ID_MID].state = BUTTON_STATE_RELEASED;
  buttons[PUSHBUTTON_ID_MID].cb = pushbutton_int_mid;

  for (int i = 0; i < 3; ++i) {
    pushbutton_init_single(&buttons[i]);
  }
}

bool pushbotton_is_pressed(int id)
{
  if (id >= PUSHBUTTON_ID_COUNT)
    return false;

  return buttons[id].state == BUTTON_STATE_PRESSED;
}
