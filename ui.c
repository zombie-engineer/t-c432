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

static float current_temp = 0.0f;
static float target_temp = 0.0f;

static int ctr = 0;

void ui_set_temp_history(const struct temp_info *th, int depth)
{
  temp_history = th;
  temp_history_depth = depth;
}

void ui_set_current_temp(float value)
{
  current_temp = value;
}

void ui_set_target_temp(float value)
{
  target_temp = value;
}

static void draw_temp_graph(void)
{
  int i;
  float norm_value;
  int pixel_x;
  int pixel_y;
  const struct temp_info *t;
  float norm_temp;
#define GRAPH_SIZE_X 64
#define GRAPH_SIZE_Y (64 - 12)

  for (int x = 0; x < 60; ++x) {
    t = &temp_history[x];
    norm_temp = (t->temp - 25.0f) / 50.0f;
    pixel_y = (int)(norm_temp * GRAPH_SIZE_Y);
    display_draw_pixel(x, pixel_y, 1);
  }

  for (int x = 0; x < GRAPH_SIZE_X; ++x) {
    int pt = x / (float)GRAPH_SIZE_X * temp_history_depth;
    t = &temp_history[pt];
    norm_temp = (t->temp - 25.0f) / 50.0f;
    pixel_y = (int)(norm_temp * GRAPH_SIZE_Y);
    display_draw_pixel(64 + x, pixel_y, 1);
  }

  norm_temp = (target_temp - 25.0f) / 50.0f;
  pixel_y = (int)(norm_temp * GRAPH_SIZE_Y);
  for (int i = 0; i < 132 / 6; ++i) {
    int x0 = i * 6;
    int x1 = x0 + 4;

    display_draw_line(x0, pixel_y, x1, pixel_y, 1);
  }
  for (int i = 0; i < (64 - 12) / 4; ++i) {
    int y0 = i * 4;
    int y1 = y0 + 1;
    display_draw_line(60, y0, 60, y1, 1);
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

  display_draw_text(44, 64 - 12, statebuf, &font_4, 1);
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
    statebuf[0] = 'P';
  else
    statebuf[0] = '-';
  statebuf[1] = 0;

  display_draw_text(88, 64 - 12, statebuf, &font_4, 1);
  display_draw_text(120, 64 - 12, cntbuf, &font_4, 1);
}

void ui_redraw(void)
{
  display_clear();
  draw_temp_graph();

  display_draw_line(40, 63, 40, 63 - 12, 1);
  display_draw_line(82, 63, 82, 63 - 12, 1);
  draw_temperature(0, current_temp);
  draw_thermostat_state();
  draw_pump_state();

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
