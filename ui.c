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
// #include "ui/adc_widget.h"
#include "ui/usb_widget.h"
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


static const uint16_t *adcbuf = NULL;
static int adcbuf_length = 0;

static int ctr = 0;

void ui_set_adcbuf(const uint16_t *array, int length)
{
  adcbuf = array;
  adcbuf_length = length;
}

static void draw_adc(void)
{
  int old_x = 0;
  int old_y = 0;
  for (int i = 0; i < adcbuf_length; ++i) {
    uint16_t value = adcbuf[i];
    uint16_t rel_value = 0;
    if (value > 2500)
      rel_value = value - 2500;

    if (rel_value > 200)
      rel_value = 200;

    float norm_value = rel_value / 200.0f;

    int y = norm_value * 63;
    int x = i;
    display_draw_line(old_x, old_y, x, y, 1);
    old_x = x;
    old_y = y;
  }
}

extern float temperature_celsius;
extern int thermostat_state;
extern int pump_state;
char textbuf_int[6];
char textbuf_fra[4];

static void update_temperature_text(void)
{
  int t_int;
  int t_fra;
  float fractional;
  char *p = textbuf_int;

  t_int = (int)temperature_celsius;
  fractional = temperature_celsius - (float)t_int;
  t_fra = (int)(roundf(fractional * 100.0f));
  itoa(t_int, textbuf_int, 10);
  p += strlen(p);
  *p++ = '.';
  *p++ = 0;
  p = textbuf_fra;
  if (t_fra > 9) {
    itoa(t_fra, p, 10);
  } else if (t_fra) {
    *p++ = '0';
    itoa(t_fra, p, 10);
  } else {
    *p++ = '0';
    *p++ = '0';
    *p++ = 0;
  }
}

static int temp_update_counter = 0;

static void draw_temperature(void)
{
  int x;

  if (temp_update_counter == 0) {
    temp_update_counter = 30;
    update_temperature_text();
  }
  temp_update_counter--;

  x = display_draw_text(10, 10, textbuf_int, &font_4, 1);
  x = display_draw_text(x, 10, textbuf_fra, &font_2, 1);
  display_draw_pixel(x + 1, 20, 1);
  display_draw_pixel(x + 2, 21, 1);
  display_draw_pixel(x + 3, 20, 1);
  display_draw_pixel(x + 2, 19, 1);
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

static void draw_pump_state(void)
{
  char statebuf[3];
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
}

void ui_redraw(void)
{
  display_clear();
  // draw_adc();
  draw_temperature();
  draw_thermostat_state();
  draw_pump_state();

  ctr++;
  if (ctr > 10)
    ctr = 0;

  display_draw_line(0, 5 + ctr, 127, 5 + ctr, 1);
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

void ui_init(void)
{
  if (main_widget_init(&root_widgets[0], focus_prev_cb, focus_next_cb)) {
    svc_call(SVC_PANIC);
  }
  if (usb_widget_init(&root_widgets[1], focus_prev_cb, focus_next_cb)) {
    svc_call(SVC_PANIC);
  }
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
  usb_widget_set_tx(buf);
}

void ui_on_rx(const char *buf)
{
  usb_widget_set_rx(buf);
}
