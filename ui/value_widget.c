#include "value_widget.h"
#include <ssd1306.h>
#include <stddef.h>
#include <stdlib.h>

struct value_priv {
  struct value_widget_value value;
  value_widget_type_t value_type;
  const struct font_descriptor *font;
};

struct value_priv val_priv_array[4];
static int val_priv_cnt = 0;

static void value_to_text(struct value_priv *p, char *buf, int bufsz)
{
  if (p->value_type == VALUE_TYPE_INT) {
    itoa(p->value.u.int_value, buf, 10);
  } else if (p->value_type == VALUE_TYPE_FLOAT) {
    buf[0] = 'f';
    buf[1] = 'l';
    buf[2] = 'o';
    buf[3] = 0;
  } else if (p->value_type == VALUE_TYPE_STRING) {
  }
}

static void value_widget_draw(struct widget *w)
{
  struct value_priv *p = w->priv;
  char buf[16];

  value_to_text(p, buf, sizeof(buf));
  int text_pos_x;
  int text_pos_y;
  int text_size_x;
  int text_size_y;

  if (!dbuf_get_text_size(buf, p->font, &text_size_x, &text_size_y, false))
    return;

  text_pos_x = w->pos_x - text_size_x / 2;
  text_pos_y = w->pos_y - text_size_y / 2;
#if 0
  if (!white) {
    dbuf_draw_filled_rect(
      text_pos_x, text_pos_y, text_pos_x + text_size_x,
      text_pos_y + text_size_y, 1);
  }
#endif
  dbuf_draw_text(text_pos_x, text_pos_y, buf, p->font, 1);
}

int value_widget_init(struct widget *w,
  value_widget_type_t value_type,
  const struct value_widget_value *initial_value,
  const struct font_descriptor *font)
{
  asm volatile ("bkpt");
  if (val_priv_cnt >= sizeof(val_priv_cnt))
  {
    return -1;
  }

  struct value_priv *p = &val_priv_array[val_priv_cnt++];
  p->value_type = value_type;
  p->value = *initial_value;
  p->font = font;
  w->priv = p;

  w->draw = value_widget_draw;
  w->on_tick = NULL;
  return 0;
}
