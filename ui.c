#include "ui.h"
#include "font.h"
#include "list.h"
#include <string.h>
#include <math.h>
#include "display_hw.h"
#include "display.h"
#include "tim.h"
#include "pushbuttons.h"
#include "ui/widget.h"
#include "ui/main_widget.h"
#include "gpio.h"
#include "common_util.h"
// #include "ui/adc_widget.h"
#if ENABLE_USB == 1
#include "ui/usb_widget.h"
#endif
#include <stdlib.h>
// #include "ui/led_widget.h"
#include "svc.h"

#define NUM_ROOT_WIDGETS 2
struct widget root_widgets[NUM_ROOT_WIDGETS];
int focus_widget_idx = 0;

#define FOCUS_WIDGET (&root_widgets[focus_widget_idx])

static void focus_next_cb(void)
{
  focus_widget_idx++;
  if (focus_widget_idx >= NUM_ROOT_WIDGETS)
    focus_widget_idx = 0;
}

static void focus_prev_cb(void)
{
  focus_widget_idx--;
  if (focus_widget_idx < 0)
    focus_widget_idx = NUM_ROOT_WIDGETS - 1;
}

void ui_tick(int ms)
{
  FOCUS_WIDGET->handle_event(FOCUS_WIDGET, UI_EVENT_TICK, ms);
}


static const struct temp_info *temp_history = NULL;
static int temp_history_depth = 0;

static float temp0 = 0.0f;
static float temp1 = 0.0f;
static float temp_int = 0.0f;

static int ctr = 0;

void ui_set_temp_history(const struct temp_info *th, int depth)
{
  temp_history = th;
  temp_history_depth = depth;
}

void ui_set_temperatures(float t0, float t1, float t_int)
{
  temp0 = t0;
  temp1 = t1;
  temp_int = t_int;
}

static void draw_temp_history(void)
{
  int i;
  float norm_value;
  int pixel_x;
  int pixel_y;

  for (int i = 0; i < temp_history_depth; ++i) {
    const struct temp_info *t = &temp_history[i];
    pixel_y = (int)((t->temp0 / 50.0f) * 30);
    display_draw_pixel(i + 0, pixel_y, 1);
    pixel_y = (int)((t->temp1 / 50.0f) * 30);
    display_draw_pixel(i + 32, pixel_y, 1);
    pixel_y = (int)((t->temp_int / 50.0f) * 30);
    display_draw_pixel(i + 64, pixel_y, 1);
  }
}

extern int thermostat_state;
extern int pump_state;

struct text_slot {
  int upd_counter;
  int x;
  int y;
  char textbuf_int[4];
  char textbuf_fra[2];
};

struct text_slot text_slots[3] = { 0 };

static void update_temperature_text(struct text_slot *ts, float value)
{
  int t_int;
  int t_fra;
  float fractional;
  char *p = ts->textbuf_int;

  if (ts->upd_counter) {
    ts->upd_counter--;
    return;
  }

  ts->upd_counter = 30;

  t_int = (float)value;
  fractional = value - (float)t_int;
  t_fra = (int)(roundf(fractional * 100.0f));
  itoa(t_int, ts->textbuf_int, 10);
  p += strlen(p);
  *p++ = '.';
  *p++ = '0' + (t_fra / 10);
  *p++ = 0;
}

static void draw_temperature(int text_slot_idx, float temperature)
{
  struct text_slot *s = &text_slots[text_slot_idx];
  int x = s->x;

  update_temperature_text(s, temperature);

  x = display_draw_text(x, s->y, s->textbuf_int, &font_4, 1);
  x = display_draw_text(x, s->y, s->textbuf_fra, &font_2, 1);
  int deg_y = s->y + 5;
  display_draw_pixel(x + 1, deg_y, 1);
  display_draw_pixel(x + 2, deg_y + 1, 1);
  display_draw_pixel(x + 3, deg_y, 1);
  display_draw_pixel(x + 2, deg_y - 1, 1);
}

static void draw_thermostat_state(void)
{
  char statebuf[3];

  if (thermostat_state == 0) {
    statebuf[0] = 'C';
    statebuf[1] = 0;
  } else if (thermostat_state == 1) {
    statebuf[0] = 'H';
    statebuf[1] = 'A';
    statebuf[2] = 0;
  } else if (thermostat_state == 2) {
    statebuf[0] = 'H';
    statebuf[1] = '-';
    statebuf[2] = 0;
  }

  display_draw_text(10, 30, statebuf, &font_4, 1);
}

int pulse_count = 0;
void ui_set_pulse_count(int value)
{
  pulse_count = value;
}

static void draw_pump_state(void)
{
  char statebuf[3];
  char cntbuf[8];

  itoa(pulse_count, cntbuf, 10);

  if (pump_state == 1)
  {
    statebuf[0] = 'P';
  }
  else
  {
    statebuf[0] = '-';
  }
    statebuf[1] = 0;

  display_draw_text(30, 30, statebuf, &font_4, 1);
  display_draw_text(50, 30, cntbuf, &font_4, 1);
}

static void draw_gpio_inputs(void)
{
  int i;
  int y;

  int x = 60;
  int indices[] = { 1, 10, 11, 12, 13, 14, 15 };

  for (i = 0; i < ARRAY_SIZE(indices); ++i) {
    y = 10;

    if (gpio_pin_is_set(GPIO_PORT_B, indices[i]))
      y += 3;

    display_draw_line(x, y, x + 3, y, 1);
    x += 4;
  }
}

void ui_redraw(void)
{
  display_clear();
  draw_temp_history();

  display_draw_line(40, 63, 40, 63 - 12, 1);
  display_draw_line(82, 63, 82, 63 - 12, 1);
  draw_temperature(0, temp0);
  draw_temperature(1, temp1);
  draw_temperature(2, temp_int);
  draw_thermostat_state();
  draw_pump_state();
  // draw_gpio_inputs();

  ctr++;
  if (ctr > 10)
    ctr = 0;

  display_draw_line(120, 5 + ctr, 127, 5 + ctr, 1);
  display_hw_flush();
}

void ui_callback_button_event_pressed(int button_id)
{
  FOCUS_WIDGET->handle_event(FOCUS_WIDGET, UI_EVENT_BUTTON_PRESSED,
    button_id);
}

void ui_callback_button_event_released(int button_id)
{
  FOCUS_WIDGET->handle_event(FOCUS_WIDGET, UI_EVENT_BUTTON_RELEASED,
    button_id);
}

static void text_slot_init(struct text_slot *s, int x, int y)
{
  s->x = x;
  s->y = y;
  s->upd_counter = 0;
  s->textbuf_int[0] = 0;
  s->textbuf_fra[0] = 0;
}

void ui_init(void)
{
  int slot_y = 63 - 12;
  text_slot_init(&text_slots[0], 2, slot_y);
  text_slot_init(&text_slots[1], 44, slot_y);
  text_slot_init(&text_slots[2], 86, slot_y);

  if (main_widget_init(&root_widgets[0], focus_prev_cb, focus_next_cb)) {
    svc_call(SVC_PANIC);
  }

#if ENABLE_USB == 1
  if (usb_widget_init(&root_widgets[1], focus_prev_cb, focus_next_cb)) {
    svc_call(SVC_PANIC);
  }
#endif
#if 0
  if (led_widget_init(&root_widgets[2], focus_prev_cb, focus_next_cb)) {
    svc_call(SVC_PANIC);
  }
#endif
#if 0
  if (adc_widget_init(&root_widgets[1], focus_prev_cb, focus_next_cb)) {
    svc_call(SVC_PANIC);
  }
#endif
}

void ui_on_set_address(void)
{
  focus_widget_idx = 2;
}

void ui_on_tx(const char *buf)
{
#if ENABLE_USB == 1
  usb_widget_set_tx(buf);
#endif
}

void ui_on_rx(const char *buf)
{
#if ENABLE_USB == 1
  usb_widget_set_rx(buf);
#endif
}
