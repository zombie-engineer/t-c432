#include "adc_widget.h"
#include "navi_header_widget.h"
#include <tim.h>
#include <ssd1306.h>
#include <scheduler.h>
#include <font.h>
#include <stdlib.h>
#include <string.h>

struct adc_widget_priv {
  struct widget navi_header;
};

struct adc_widget_priv adc_widget_priv;

static void adc_widget_handle_event(struct widget *w, ui_event_type event,
  int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
    case UI_EVENT_BUTTON_RELEASED:
      adc_widget_priv.navi_header.handle_event(
        &adc_widget_priv.navi_header, event, param);
      break;
    case UI_EVENT_TICK:
    default:
      break;
  }
}

void adc_widget_draw(struct widget *w)
{
  adc_widget_priv.navi_header.draw(&adc_widget_priv.navi_header);
}

int adc_widget_init(struct widget *w, nextprev_fn prev, nextprev_fn next)
{
  struct adc_widget_priv *p = &adc_widget_priv;
  struct widget *navi = &adc_widget_priv.navi_header;

  w->pos_x = 0;
  w->pos_y = 0;
  w->size_x = 128;
  w->size_y = 64;

  navi_header_widget_init(navi,
    0,
    64 - 10,
    128,
    10,
    prev,
    next,
    "adc",
    &font_1
  );

  w->priv = p;
  w->draw = adc_widget_draw;
  w->handle_event = adc_widget_handle_event;
  return 0;
}
