#include "main_widget.h"
#include "bar_widget.h"
#include "navi_header_widget.h"
#include "value_widget.h"
#include <tim.h>
#include <ssd1306.h>
#include <scheduler.h>
#include <font.h>
#include <stdlib.h>
#include <string.h>

struct main_widget_priv {
  struct widget navi_header;
  struct widget value1;
  struct widget value2;
  struct widget value3;
  struct widget vert_bar;
};

static struct main_widget_priv main_widget_priv;
static int frame_counter = 0;
static int duration;

static void draw_tim2_cntr(int x, int y)
{
  char buf[32];
  int val = tim2_read_counter_value() >> 4;
  const char *prefix_str = "TIM2_TCNT:";
  int text_size_x;
  int text_size_y;
  itoa(val, buf, 10);

  if (!dbuf_get_text_size(prefix_str, &font_1, &text_size_x, &text_size_y, false))
    return;

  x = dbuf_draw_text(x - text_size_x, y - text_size_y / 2, prefix_str, &font_1, 1);
  dbuf_draw_text(x, y - text_size_y / 2, buf, &font_2, 1);

  itoa(sched_stats.task_switches, buf, 10);
  dbuf_draw_text(x, y + 15, buf, &font_2, 1);
}

static void draw_voltmeter(int x, int y, int volt_int, int volt_frac)
{
  char b[32];
  char *p = b;
  itoa(volt_int, p, 10);
  p += strlen(p);
  *p++ = '.';
  itoa(volt_frac, p, 10);
  p += strlen(p);
  x = dbuf_draw_text(x, y, "V:", &font_3, 1);
  dbuf_draw_text(x, y, b, &font_3, 1);
}

static void draw_blinker_icon(int x, int y, int sz, int num_frames, int interval)
{
  if (!duration && !(frame_counter % interval)) {
    duration = num_frames;
  }
  if (duration) {
    dbuf_draw_filled_rect(x, y, x + sz, y + sz, 1);
    duration--;
  }
}

static void main_on_tick(struct widget *w, int tick_ms)
{
  struct main_widget_priv *p = &main_widget_priv;
  p->navi_header.handle_event(&p->navi_header, UI_EVENT_TICK, tick_ms);

  struct value_widget_value v = {
    .u.int_value = sched_stats.task_switches
  };

  value_widget_set_value(&p->value1, &v);
  v.u.int_value = sched_stats.task_sleeps;
  value_widget_set_value(&p->value2, &v);
  v.u.int_value = sched_stats.num_runnable_tasks;
  value_widget_set_value(&p->value3, &v);
}

static void main_widget_draw(struct widget *w)
{
  struct main_widget_priv *p = w->priv;

  /* < title  >*/
  p->navi_header.draw(&p->navi_header);

  p->value1.draw(&p->value1);
  p->value2.draw(&p->value2);
  p->value3.draw(&p->value3);
}

static void main_widget_handle_event(struct widget *w, ui_event_type event,
  int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
    case UI_EVENT_BUTTON_RELEASED:
      main_widget_priv.navi_header.handle_event(
        &main_widget_priv.navi_header, event, param);
      break;
    case UI_EVENT_TICK:
      main_on_tick(w, param);
      break;
    default:
      break;
  }
}

int main_widget_init(struct widget *w, nextprev_fn prev, nextprev_fn next)
{
  struct widget *navi = &main_widget_priv.navi_header;
  struct widget *b = &main_widget_priv.vert_bar;
  struct widget *v1 = &main_widget_priv.value1;
  struct widget *v2 = &main_widget_priv.value2;
  struct widget *v3 = &main_widget_priv.value3;

  w->pos_x = 0;
  w->pos_y = 0;
  w->size_x = 128;
  w->size_y = 64;

  bar_widget_init(b);
  b->pos_x = 110;
  b->pos_y = 0;
  b->size_x = 10;
  b->size_y = 48,

  navi_header_widget_init(navi,
    0,
    w->size_y - 10,
    w->size_x,
    10,
    prev,
    next,
    "scheduler",
    &font_1
  );

  struct value_widget_value value_initial = {
    .u.int_value = 0
  };

  value_widget_init(v1,
    w->size_x / 2,
    w->size_y - 30,
    "sw",
    VALUE_TYPE_INT,
    &value_initial,
    &font_1);

  value_widget_init(v2,
    w->size_x / 2,
    w->size_y - 40,
    "sl",
    VALUE_TYPE_INT,
    &value_initial,
    &font_1);

  value_widget_init(v3,
    w->size_x / 2,
    w->size_y - 50,
    "tr",
    VALUE_TYPE_INT,
    &value_initial,
    &font_1);

  w->priv = &main_widget_priv;
  w->draw = main_widget_draw;
  w->handle_event = main_widget_handle_event;
  return 0;
}
