#include "led_widget.h"
#include "navi_header_widget.h"
#include "bar_widget.h"
#include <display.h>
#include <font.h>
#include <stdlib.h>
#include <string.h>

struct led_widget_priv {
  struct widget navi_header;
  struct widget bar;
};

struct led_widget_priv led_widget_priv;
#define LED_PARAM_NO 0
#define LED_PARAM_SLOW 1
#define LED_PARAM_BRIGHTNESS 2

static char led_param_current = LED_PARAM_NO;

static void led_on_tick(struct widget *w, int tick_ms)
{
  struct led_widget_priv *p = w->priv;
  p->navi_header.handle_event(&p->navi_header, UI_EVENT_TICK, tick_ms);
}

extern char led_param_slow;
extern char led_param_brightness;

static void led_widget_on_left_button_pressed(struct led_widget_priv *p)
{
  switch (led_param_current)
  {
    case LED_PARAM_SLOW:
      if (led_param_slow > 2)
        led_param_slow--;
      break;
    case LED_PARAM_BRIGHTNESS:
      if (led_param_brightness > 1)
        led_param_brightness--;
      break;
  }
}

static void led_widget_on_mid_button_pressed(struct led_widget_priv *p)
{
  
}

static void led_widget_on_right_button_pressed(struct led_widget_priv *p)
{
  switch (led_param_current)
  {
    case LED_PARAM_SLOW:
        led_param_slow++;
      break;
    case LED_PARAM_BRIGHTNESS:
        led_param_brightness++;
      break;
  }
}

static void led_widget_on_button_pressed(struct led_widget_priv *p, int button_id)
{
  switch(button_id)
  {
    case PUSHBUTTON_ID_LEFT:
      led_widget_on_left_button_pressed(p);
      break;
    case PUSHBUTTON_ID_MID:
      led_widget_on_mid_button_pressed(p);
      break;
    case PUSHBUTTON_ID_RIGHT:
      led_widget_on_right_button_pressed(p);
      break;
    default:
      break;
  }
}

static void led_widget_on_button_action(struct led_widget_priv *p,
  ui_event_type event, int button_id)
{
  if (event == UI_EVENT_BUTTON_RELEASED)
    return;

  if (button_id == PUSHBUTTON_ID_MID)
  {
    if (led_param_current == LED_PARAM_NO) {
      led_param_current = LED_PARAM_SLOW;
    }
    else if (led_param_current == LED_PARAM_SLOW) {
      led_param_current = LED_PARAM_BRIGHTNESS;
    } else {
      led_param_current = LED_PARAM_NO;
    }
    return;
  }

  if (led_param_current == LED_PARAM_NO) {
    p->navi_header.handle_event(&p->navi_header, event, button_id);
  } else {
    led_widget_on_button_pressed(p, button_id);
  }
}

static void led_widget_handle_event(struct widget *w, ui_event_type event,
  int param)
{
  struct led_widget_priv *p = w->priv;

  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
    case UI_EVENT_BUTTON_RELEASED:
      led_widget_on_button_action(p, event, param);
      break;
    case UI_EVENT_TICK:
    default:
      led_on_tick(w, param);
      break;
  }
}

char led_intensities[44];

void ui_led_pixel(int idx, uint8_t r, uint8_t g, uint8_t b)
{
  if (idx > 43)
    asm volatile ("bkpt");

  led_intensities[idx] = (((uint16_t)r + g + b)) / 3.0f;
}

void led_widget_draw(struct widget *w)
{
  led_widget_priv.navi_header.draw(&led_widget_priv.navi_header);
  const char *led_param_name = NULL;
  char led_param_buf[8];

  if (led_param_current == LED_PARAM_SLOW) {
    led_param_name = "slow:";
    itoa(led_param_slow, led_param_buf, 10);
  } else if (led_param_current == LED_PARAM_BRIGHTNESS) {
    led_param_name = "brig:";
    itoa(led_param_brightness, led_param_buf, 10);
  }

  if (led_param_name) {
//  led_widget_priv.bar.draw(&led_widget_priv.bar);
    display_draw_text(20, 20, led_param_name, &font_1, 1);
    display_draw_text(20 + 30, 20, led_param_buf, &font_1, 1);
  }

  for (int i = 0 ; i < 44; ++i) {
    int x = (int)(i / 43.0f * 127);
    int y1 = 1 + (led_intensities[i] / 255.0 * (63.0f - 30));
    display_draw_line(x, 0, x, y1, 1);
  }
}

int led_widget_init(struct widget *w, nextprev_fn prev, nextprev_fn next)
{
  int ret;
  struct led_widget_priv *p = &led_widget_priv;
  struct widget *b = &p->bar;
  struct widget *navi = &led_widget_priv.navi_header;

  w->pos_x = 0;
  w->pos_y = 0;
  w->size_x = 128;
  w->size_y = 64;

  ret = navi_header_widget_init(navi,
    0,
    64 - 10,
    128,
    10,
    prev,
    next,
    "led",
    &font_1
  );

  if (ret)
    return ret;

  ret = bar_widget_init(b);
  if (ret)
    return ret;

  b->pos_x = 110;
  b->pos_y = 0;
  b->size_x = 10;
  b->size_y = 48,

  w->priv = p;
  w->draw = led_widget_draw;
  w->handle_event = led_widget_handle_event;
  return 0;
}
