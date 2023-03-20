#include "bar_widget.h"
#include <common_util.h>
#include <display.h>

struct bar_widget_priv {
  int level;
};

void bar_widget_draw(struct widget *w)
{
  struct bar_widget_priv *p;
  p = w->priv;
  display_draw_rect(
    w->pos_x, w->pos_y,
    w->pos_x + w->size_x,
    w->pos_y + w->size_y, 1);

  display_draw_hatched_rect(
    w->pos_x + 2,
    w->pos_y,
    w->pos_x + w->size_x - 1,
    w->pos_y + p->level,
    1);

  display_draw_line(
    w->pos_x + 2,
    w->pos_y + p->level,
    w->pos_x + w->size_x - 1,
    w->pos_y + p->level,
    1);
}

static struct bar_widget_priv bar_widget_priv_arr[3];
static int bar_widget_last_free_idx = 0;

void bar_widget_handle_event(struct widget *w, ui_event_type ev, int param)
{
}

int bar_widget_init(struct widget *w)
{
  struct bar_widget_priv *p = NULL;
  if (bar_widget_last_free_idx >= ARRAY_SIZE(bar_widget_priv_arr))
    return -1;

  p = &bar_widget_priv_arr[bar_widget_last_free_idx++];

  p->level = 0;
  w->priv = p;
  w->draw = bar_widget_draw;
  w->handle_event = bar_widget_handle_event;
  w->pos_x = 0;
  w->pos_y = 0;
  w->size_x = 128;
  w->size_y = 64;
  return 0;
}

void bar_widget_set_level(struct widget *w, char level)
{
  struct bar_widget_priv *b = w->priv;
  b->level = level;
}
