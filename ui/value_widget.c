#include "value_widget.h"
#include <ssd1306.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct value_priv {
  const char *name_string;
  struct value_widget_value value;
  value_widget_type_t value_type;
  const struct font_descriptor *font;
};

struct value_priv val_priv_array[4];
static int val_priv_cnt = 0;

/*
 * 0xffffffff = 4294967295 (4 billion) is the most lengthy uint32_t value
 * 0x80000000 = -2147483648 (-2billion) is the most lengthy int32_t value
 * given a - sign we need 10 chars for 1-9 billion + 1 char for minus sign
 * plus we need a zero termination, 10 + 1 + 1 = 12 is a minimum char buffer
 */
#define MIN_VALUE_BUFFER_LEN 12
#define MAX_NAME_LEN 5

static bool value_to_text(struct value_priv *p,
  char *out_buf,
  int bufsz,
  int *out_len)
{
  if (!p || !out_buf || !out_len || bufsz < MIN_VALUE_BUFFER_LEN)
    return false;

  if (p->value_type == VALUE_TYPE_INT) {
    itoa(p->value.u.int_value, out_buf, 10);
    p->value.u.int_value++;
    *out_len = strlen(out_buf);
    return true;
  } else if (p->value_type == VALUE_TYPE_FLOAT) {
    out_buf[0] = 'f';
    out_buf[1] = 'l';
    out_buf[2] = 'o';
    out_buf[3] = 0;
  } else if (p->value_type == VALUE_TYPE_STRING) {
  }
  return false;
}

static void value_widget_draw(struct widget *w)
{
  struct value_priv *p = w->priv;
  const char *srcptr = p->name_string;

  char stringbuf[MAX_NAME_LEN + MIN_VALUE_BUFFER_LEN];
  char *dstptr = stringbuf;
  int len;
  int text_pos_x;
  int text_pos_y;
  int text_size_x;
  int text_size_y;

//  asm volatile("bkpt");
  if (srcptr) {
    while(1) {
      char c = *srcptr++;
      if (!c)
        break;
      *dstptr++ = c;
    }
    *dstptr++ = ':';
  }

  if (!value_to_text(p, dstptr, sizeof(stringbuf) - (dstptr - stringbuf),
    &len)) {
    stringbuf[0] = '<';
    stringbuf[1] = 'e';
    stringbuf[2] = 'r';
    stringbuf[3] = 'r';
    stringbuf[4] = '>';
    stringbuf[5] = 0;
  }

  if (!dbuf_get_text_size(stringbuf, p->font, &text_size_x, &text_size_y,
    false))
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
  dbuf_draw_text(text_pos_x, text_pos_y, stringbuf, p->font, 1);
}

void value_widget_set_value(struct widget *w,
  const struct value_widget_value *v)
{
  struct value_priv *p = w->priv;
  p->value = *v;
}

int value_widget_init(struct widget *w,
  const char *name_string,
  value_widget_type_t value_type,
  const struct value_widget_value *initial_value,
  const struct font_descriptor *font)
{
//  asm volatile ("bkpt");
  if (val_priv_cnt >= sizeof(val_priv_cnt))
  {
    return -1;
  }

  struct value_priv *p = &val_priv_array[val_priv_cnt++];
  p->value_type = value_type;
  p->name_string = name_string;
  p->value = *initial_value;
  p->font = font;
  w->priv = p;

  w->draw = value_widget_draw;
  w->on_tick = NULL;
  return 0;
}
