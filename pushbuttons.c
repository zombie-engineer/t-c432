#include "pushbuttons.h"
#include "gpio.h"
#include "exti.h"
#include "nvic.h"
#include "common_util.h"
#include "config.h"
#include "pin_config.h"

#define BUTTON_STATE_PRESSED 0
#define BUTTON_STATE_RELEASED 1

struct button_info {
  uint8_t button_id;
  uint8_t pin;
  uint8_t port;
  bool pressed_level_high;
  uint8_t state;
  exti_interrupt_callback cb;
};

struct button_info buttons[PUSHBUTTON_ID_COUNT] = { 0 };

struct pushbutton_event_cb_info {
  pushbutton_event_callback_t cb;
  uint8_t button_id;
  uint8_t event;
};

static struct pushbutton_event_cb_info pushbutton_callbacks[4] = { 0 };

static void pushbutton_run_event_callbacks(int button_id, int event)
{
  struct pushbutton_event_cb_info *info;

  for (int i = 0; i < ARRAY_SIZE(pushbutton_callbacks); ++i) {
    info = &pushbutton_callbacks[i];
    if (info->button_id == button_id && info->event == event && info->cb) {
      info->cb(button_id, event);
    }
  }
}

bool pushbutton_register_callback(int button_id, int event,
  pushbutton_event_callback_t cb)
{
  struct pushbutton_event_cb_info *info;

  for (int i = 0; i < ARRAY_SIZE(pushbutton_callbacks); ++i) {
    info = &pushbutton_callbacks[i];
    if (!info->cb) {
      info->cb = cb;
      info->button_id = button_id;
      info->event = event;
      return true;
    }
  }
  return false;
}

static bool pushbutton_is_pressed(const struct button_info *b)
{
  return gpio_pin_is_set(b->port, b->pin) == b->pressed_level_high;
}

static void pushbutton_signal(int button_idx)
{
  struct button_info *b;
  if (button_idx >= ARRAY_SIZE(buttons))
    return;

  b = &buttons[button_idx];

  if (pushbutton_is_pressed(b)) {
    if (b->state == BUTTON_STATE_RELEASED) {
      pushbutton_run_event_callbacks(b->button_id, PUSHBUTTON_EVENT_PRESSED);
      b->state = BUTTON_STATE_PRESSED;
    }
  } else {
    if (b->state == BUTTON_STATE_PRESSED) {
      pushbutton_run_event_callbacks(b->button_id, PUSHBUTTON_EVENT_RELEASED);
      b->state = BUTTON_STATE_RELEASED;
    }
  }
}

static void pushbutton_int_main(void)
{
  pushbutton_signal(PUSHBUTTON_ID_MAIN);
}

void pushbutton_init_single(struct button_info *b)
{
  gpio_setup(b->port, b->pin, GPIO_MODE_INPUT, GPIO_CNF_IN_PULLUP_PULLDOWN);
  if (b->pressed_level_high)
      gpio_odr_modify(b->port, b->pin, 0);
  else
      gpio_odr_modify(b->port, b->pin, 1);
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
  buttons[PUSHBUTTON_ID_MAIN].button_id = PUSHBUTTON_ID_MAIN;
  buttons[PUSHBUTTON_ID_MAIN].pin = BUTTON_KEY_MAIN_GPIO_PIN;
  buttons[PUSHBUTTON_ID_MAIN].port = BUTTON_KEY_MAIN_GPIO_PORT;
  buttons[PUSHBUTTON_ID_MAIN].state = BUTTON_STATE_RELEASED;
  buttons[PUSHBUTTON_ID_MAIN].cb = pushbutton_int_main;
  buttons[PUSHBUTTON_ID_MAIN].pressed_level_high = false;

  for (int i = 0; i < 1; ++i) {
    pushbutton_init_single(&buttons[i]);
  }
}

bool pushbotton_is_pressed(int id)
{
  if (id >= PUSHBUTTON_ID_COUNT)
    return false;

  return buttons[id].state == BUTTON_STATE_PRESSED;
}
