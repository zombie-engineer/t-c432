#include "usb_widget.h"
#include "navi_header_widget.h"
#include "text_widget.h"
#include <tim.h>
#include <ssd1306.h>
#include <scheduler.h>
#include <font.h>
#include <stdlib.h>
#include <string.h>

struct usb_widget_priv {
  struct widget navi_header;
  struct widget text;
};

struct usb_widget_priv usb_widget_priv;

static void usb_widget_handle_event(struct widget *w, ui_event_type event,
  int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
    case UI_EVENT_BUTTON_RELEASED:
      usb_widget_priv.navi_header.handle_event(
        &usb_widget_priv.navi_header, event, param);
      break;
    case UI_EVENT_TICK:
    default:
      break;
  }
}

void usb_widget_draw(struct widget *w)
{
  usb_widget_priv.navi_header.draw(&usb_widget_priv.navi_header);
  usb_widget_priv.text.draw(&usb_widget_priv.text);
}

int usb_widget_init(struct widget *w, nextprev_fn prev, nextprev_fn next)
{
  struct usb_widget_priv *p = &usb_widget_priv;
  struct widget *navi = &usb_widget_priv.navi_header;

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
    "usb",
    &font_1
  );

  text_widget_init(&usb_widget_priv.text,
  w->size_x / 2,
  w->size_y / 2,
  30,
  8,
  "some",
  &font_1);

  w->priv = p;
  w->draw = usb_widget_draw;
  w->handle_event = usb_widget_handle_event;
  return 0;
}

void usb_widget_set_rx(const char *buf)
{
  text_widget_set_text(&usb_widget_priv.text, buf);
}

void usb_widget_set_tx(const char *buf)
{
  struct widget value1;
}
