#include "navi_header_widget.h"
#include "arrow_button_widget.h"
#include "text_widget.h"
#include <common_util.h>
#include <font.h>

struct navi_header_widget_priv {
  struct widget text;
  struct widget arrow_button_left;
  struct widget arrow_button_right;
  nextprev_fn next;
  nextprev_fn prev;
};

struct navi_header_widget_priv navi_priv_array[4];
static int navi_priv_cnt = 0;

void navi_header_draw(struct widget *w)
{
  struct navi_header_widget_priv *p = w->priv;
  p->arrow_button_left.draw(&p->arrow_button_left);
  p->text.draw(&p->text);
  p->arrow_button_right.draw(&p->arrow_button_right);
}

static void on_left_clicked(struct widget *w)
{
  struct navi_header_widget_priv *p = w->priv;
  arrow_button_widget_activate(&p->arrow_button_left);
  p->prev();
}

static void on_mid_clicked(struct widget *w)
{
  struct navi_header_widget_priv *p = w->priv;
  text_widget_activate(&p->text);
}

static void on_right_clicked(struct widget *w)
{
  struct navi_header_widget_priv *p = w->priv;
  arrow_button_widget_activate(&p->arrow_button_right);
  p->next();
}


static void navi_header_on_button_pressed(struct widget *w, int button_id)
{
  switch(button_id)
  {
    case PUSHBUTTON_ID_LEFT:
      on_left_clicked(w);
      break;
    case PUSHBUTTON_ID_MID:
      on_mid_clicked(w);
      break;
    case PUSHBUTTON_ID_RIGHT:
      on_right_clicked(w);
      break;
    default:
      break;
  }
}

static void navi_header_on_tick(struct widget *w, int tick_ms)
{
  struct navi_header_widget_priv *p = w->priv;

  p->arrow_button_left.handle_event(&p->arrow_button_left, UI_EVENT_TICK,
    tick_ms);

  p->arrow_button_right.handle_event(&p->arrow_button_right, UI_EVENT_TICK,
    tick_ms);

  p->text.handle_event(&p->text, UI_EVENT_TICK, tick_ms);
}

static void navi_header_on_button_released(struct widget *w, int button_id)
{
}

static void navi_header_handle_event(struct widget *w, ui_event_type event,
  int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
      navi_header_on_button_pressed(w, param);
      break;
    case UI_EVENT_BUTTON_RELEASED:
      navi_header_on_button_released(w, param);
      break;
    case UI_EVENT_TICK:
      navi_header_on_tick(w, param);
      break;
    default:
      break;
  }
}

int navi_header_widget_init(struct widget *w,
  int x,
  int y,
  int sx,
  int sy,
  nextprev_fn prev,
  nextprev_fn next,
  const char *name,
  const struct font_descriptor *font
  )
{
  if (navi_priv_cnt >= ARRAY_SIZE(navi_priv_array))
    return -1;

  struct navi_header_widget_priv *p = &navi_priv_array[navi_priv_cnt++];
  p->next = next;
  p->prev = prev;

  w->pos_x = x;
  w->pos_y = y;
  w->size_x = sx;
  w->size_y = sy;
  w->priv = p;

  arrow_button_widget_init(&p->arrow_button_left,
    w->pos_x + 3,
    w->pos_y - 3,
    3,
    5,
    ARROW_BUTTON_TYPE_LEFT);

  arrow_button_widget_init(&p->arrow_button_right,
    w->pos_x - 4,
    w->pos_y - 3,
    3,
    5,
    ARROW_BUTTON_TYPE_RIGHT);

  text_widget_init(&p->text,
    w->pos_x + w->size_x / 2,
    w->pos_y - 3,
    0,
    0,
    name, font);

  w->handle_event = navi_header_handle_event;
  w->draw = navi_header_draw;
  return 0;
}
