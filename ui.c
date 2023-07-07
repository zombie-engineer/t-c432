#include "ui.h"
#include "font.h"
#include "list.h"
#include <string.h>
#include "display_hw.h"
#include "display.h"
#include "tim.h"
#include "pushbuttons.h"
#include "ui/widget.h"
#include "ui/main_widget.h"
// #include "ui/adc_widget.h"
#include "ui/usb_widget.h"
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


extern uint16_t adc_buf[128];
void ui_redraw(void)
{
  display_clear();
  int old_x = 0;
  int old_y = 0;
  for (int i = 0; i < 128; ++i) {
    uint16_t value = adc_buf[i];
    uint16_t rel_value = 0;
    if (value > 2500)
      rel_value = value - 2500;

    if (rel_value > 200)
      rel_value = 200;

    float norm_value = rel_value / 200.0f;
#if 0
    norm_value -= 0.61f;
    if (norm_value < 0)
      norm_value = 0;
    norm_value *= 10;
    if (norm_value > 1)
      norm_value = 1;
#endif

    int y = norm_value * 63;
    int x = i;
    // display_draw_pixel(i, y, 1);
    display_draw_line(old_x, old_y, x, y, 1);
    old_x = x;
    old_y = y;
  }

  display_draw_line(0, 5, 127, 5, 1);
//  FOCUS_WIDGET->draw(FOCUS_WIDGET);
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
