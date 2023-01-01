#include "bar_widget.h"
#include <ssd1306.h>

struct bar_widget_priv {
  int level;
};

void bar_widget_draw(struct widget *w)
{
  struct bar_widget_priv *p;
  p = w->data;
  dbuf_draw_rect(
    w->pos_x, w->pos_y,
    w->pos_x + w->size_x,
    w->pos_y + w->size_y, 1);

  dbuf_draw_hatched_rect(
    w->pos_x + 2,
    w->pos_y,
    w->pos_x + w->size_x - 1,
    w->pos_y + p->level,
    1);

  dbuf_draw_line(
    w->pos_x + 2,
    w->pos_y + p->level,
    w->pos_x + w->size_x - 1,
    w->pos_y + p->level,
    1);
}

static struct bar_widget_priv bar_widget_priv;

void bar_widget_handle_event(struct widget *w, ui_event_type ev, int param)
{
}

void bar_widget_init(struct widget *w)
{
  bar_widget_priv.level = 0;
  w->data = &bar_widget_priv;
  w->draw = bar_widget_draw;
  w->handle_event = bar_widget_handle_event;
  w->pos_x = 0;
  w->pos_y = 0;
  w->size_x = 128;
  w->size_y = 64;
}
