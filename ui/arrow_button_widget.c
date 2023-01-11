#include "arrow_button_widget.h"
#include <common_util.h>
#include <ssd1306.h>

struct arrow_button_priv {
  int active_count;
  arrow_button_type_t type;
};

struct arrow_button_priv ab_priv_array[4];
static int ab_priv_cnt = 0;

static inline void draw_arrow_left(int x, int y, int sx, int sy, bool active)
{
  int white = 1;

  dbuf_draw_pixel(x + 2, y - 0, white);
  dbuf_draw_pixel(x + 1, y - 1, white);
  dbuf_draw_pixel(x + 0, y - 2, white);
  dbuf_draw_pixel(x + 1, y - 3, white);
  dbuf_draw_pixel(x + 2, y - 4, white);
}

static inline void draw_arrow_right(int x, int y, int sx, int sy, bool white)
{
  dbuf_draw_pixel(x + 0, y - 0, white);
  dbuf_draw_pixel(x + 1, y - 1, white);
  dbuf_draw_pixel(x + 2, y - 2, white);
  dbuf_draw_pixel(x + 1, y - 3, white);
  dbuf_draw_pixel(x + 0, y - 4, white);
}

static void arrow_button_widget_on_tick(struct widget *w, int tick_ms)
{
  struct arrow_button_priv *p = w->priv;

  if (p->active_count)
    p->active_count--;
}

static void arrow_button_widget_draw(struct widget *w)
{
  struct arrow_button_priv *p = w->priv;

  int white = 1;
  if (p->active_count)
  {
    dbuf_draw_filled_rect(
      w->pos_x,
      w->pos_y,
      w->pos_x + w->size_x,
      w->pos_y + w->size_y, white);

    white = 0;
  }

  if (p->type == ARROW_BUTTON_TYPE_LEFT) {
    draw_arrow_left(w->pos_x, w->pos_y, w->size_x, w->size_y,
      white);
  } else if (p->type == ARROW_BUTTON_TYPE_RIGHT) {
    draw_arrow_right(w->pos_x, w->pos_y, w->size_x, w->size_y,
      white);
  }
}

int arrow_button_widget_init(struct widget *w, arrow_button_type_t type)
{
  if (ab_priv_cnt >= sizeof(ab_priv_cnt))
  {
    return -1;
  }

  struct arrow_button_priv *p = &ab_priv_array[ab_priv_cnt++];
  p->active_count = 0;
  p->type = type;
  w->priv = p;
  w->draw = arrow_button_widget_draw;
  w->on_tick = arrow_button_widget_on_tick;
}

void arrow_button_widget_activate(struct widget *w)
{
  struct arrow_button_priv *p = w->priv;
  p->active_count = 4;
}
