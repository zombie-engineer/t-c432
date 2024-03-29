#include "text_widget.h"
#include <stddef.h>
#include <display.h>
#include <font.h>
#include <common_util.h>
#include <string.h>

#define UI_TEXT_WIDGET_MAX_TEXT_LEN 8
struct text_priv {
  char text[UI_TEXT_WIDGET_MAX_TEXT_LEN];
  const struct font_descriptor *font;
  int active_count;
};

struct text_priv t_priv_array[5];
static int t_priv_cnt = 0;

static void text_widget_draw(struct widget *w)
{
  struct text_priv *p = w->priv;
  int white = p->active_count ? 0 : 1;
  int text_pos_x;
  int text_pos_y;
  int text_size_x;
  int text_size_y;

  if (!display_get_text_size(p->text, p->font, &text_size_x, &text_size_y, false))
    return;

  text_pos_x = w->pos_x - text_size_x / 2;
  text_pos_y = w->pos_y - text_size_y / 2;
  if (!white) {
    display_draw_filled_rect(
      text_pos_x, text_pos_y, text_pos_x + text_size_x,
      text_pos_y + text_size_y, 1);
  }
  display_draw_text(text_pos_x, text_pos_y, p->text, p->font, white);
}

static void text_widget_on_tick(struct widget *w, int tick_ms)
{
  struct text_priv *p = w->priv;
  if (p->active_count) {
    p->active_count--;
  }
}

static void text_handle_event(struct widget *w, ui_event_type event, int param)
{
  switch(event)
  {
    case UI_EVENT_TICK:
      text_widget_on_tick(w, param);
      break;
    default:
      break;
  }
}

void text_widget_activate(struct widget *w)
{
  struct text_priv *p = w->priv;
  p->active_count = 3;
}

int text_widget_init(struct widget *w,
  int x,
  int y,
  int sx,
  int sy,
  const char *text,
  const struct font_descriptor *font)
{
  if (t_priv_cnt >= ARRAY_SIZE(t_priv_array)) {
    return -1;
  }

  struct text_priv *p = &t_priv_array[t_priv_cnt++];

  strncpy(p->text, text, UI_TEXT_WIDGET_MAX_TEXT_LEN);
  p->font = font;

  w->pos_x = x;
  w->pos_y = y;
  w->size_x = sx;
  w->size_y = sy;
  w->draw = text_widget_draw;
  w->handle_event = text_handle_event;
  w->priv = p;
  return 0;
}

void text_widget_set_text(struct widget *w, const char *t)
{
  struct text_priv *p = w->priv;
  strncpy(p->text, t, UI_TEXT_WIDGET_MAX_TEXT_LEN);
}
