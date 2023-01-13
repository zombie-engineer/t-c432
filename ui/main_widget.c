#include "main_widget.h"
#include "bar_widget.h"
#include "arrow_button_widget.h"
#include <tim.h>
#include <ssd1306.h>
#include <font.h>
#include <stdlib.h>
#include <string.h>

struct main_widget_priv {
  struct widget vert_bar;
  struct widget arrow_button_left;
  struct widget arrow_button_right;
  int screen_size_x;
  int screen_size_y;
};

static struct main_widget_priv main_widget_priv;
static int frame_counter = 0;
static int duration;

static void draw_tim2_cntr(int x, int y)
{
  char counterbuf[32];
  int val = tim2_read_counter_value() >> 4;
  itoa(val, counterbuf, 10);
  x = dbuf_draw_text(x, y, "TIM2_TCNT:", &font_1, 1);
  dbuf_draw_text(x, y, counterbuf, &font_2, 1);
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

static void on_left_clicked()
{
  arrow_button_widget_activate(&main_widget_priv.arrow_button_left);
}

static void on_mid_clicked()
{
  arrow_button_widget_activate(&main_widget_priv.arrow_button_right);
}

static void on_right_clicked()
{
  struct widget *b = &main_widget_priv.vert_bar;
  b->pos_x++;
}

void main_on_tick(struct widget *w, int tick_ms)
{
  struct main_widget_priv *p = &main_widget_priv;
  p->arrow_button_left.on_tick(&p->arrow_button_left, tick_ms);
  p->arrow_button_right.on_tick(&p->arrow_button_right, tick_ms);
}

void main_widget_draw(struct widget *w)
{
  struct main_widget_priv *p = w->priv;
  const char *title_str = "main";
  int text_size_x;
  int text_size_y;
  int text_pos_x;
  int text_pos_y;
  int y0, y1, y2;
  int x0, x1;

  if (!dbuf_get_text_size(title_str, &font_1, &text_size_x, &text_size_y, false))
    return;

  text_pos_x = main_widget_priv.screen_size_x / 2 - text_size_x / 2;
  text_pos_y = main_widget_priv.screen_size_y - text_size_y;
  dbuf_draw_text(text_pos_x, text_pos_y, title_str, &font_1, 1);

  y0 = main_widget_priv.screen_size_y - 3;
  y1 = y0 - 1;
  y2 = y1 - 1;

  /* < */
  main_widget_priv.arrow_button_left.draw(
    &main_widget_priv.arrow_button_left);

  main_widget_priv.arrow_button_right.draw(
    &main_widget_priv.arrow_button_right);

  return;
  p->vert_bar.draw(&p->vert_bar);

  return;
  draw_blinker_icon(89, 6, 5, 3, 13);
  int y;
  float adc_normalized = 0.0f;// (float)last_adc / 4096;
  int level = adc_normalized * 64;
  int num_volt = adc_normalized * 3300;
  int first = num_volt / 1000;
  int next = num_volt - first * 1000;
//  draw_voltmeter(2, 35, first, next);
//
//  y = 40;
//  dbuf_draw_line(64, y, 64, y + 3, 1);
//  y = 20;
//  dbuf_draw_line(64, y, 64, y + 3, 1);
//  y = 5;
//  dbuf_draw_line(64, y, 64, y + 3, 1);
//  draw_tim2_cntr(10, 50);
}


static void main_widget_on_button_pressed(struct widget *w, int button_id)
{
  switch(button_id)
  {
    case PUSHBUTTON_ID_LEFT:
      on_left_clicked();
      break;
    case PUSHBUTTON_ID_MID:
      on_mid_clicked();
      break;
    case PUSHBUTTON_ID_RIGHT:
      on_right_clicked();
      break;
    default:
      break;
  }
}

static void main_widget_on_button_released(struct widget *w, int button_id)
{
}

static void main_widget_handle_event(struct widget *w, ui_event_type event,
  int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
      main_widget_on_button_pressed(w, param);
      break;
    case UI_EVENT_BUTTON_RELEASED:
      main_widget_on_button_released(w, param);
      break;
    default:
      break;
  }
}

int main_widget_init(struct widget *w)
{
  struct widget *b = &main_widget_priv.vert_bar;
  struct widget *larrow = &main_widget_priv.arrow_button_left;
  struct widget *rarrow = &main_widget_priv.arrow_button_right;

  if (!dbuf_get_frame_size(
    &main_widget_priv.screen_size_x,
    &main_widget_priv.screen_size_y))
  {
    return -1;
  }

  bar_widget_init(b);
  b->pos_x = 110;
  b->pos_y = 0;
  b->size_x = 10;
  b->size_y = 48,

  arrow_button_widget_init(larrow, ARROW_BUTTON_TYPE_LEFT);
  larrow->pos_x = 3;
  larrow->pos_y = main_widget_priv.screen_size_y - 3;
  larrow->size_x = 3;
  larrow->size_y = 5;

  arrow_button_widget_init(rarrow, ARROW_BUTTON_TYPE_RIGHT);
  rarrow->pos_x = main_widget_priv.screen_size_x - 4;
  rarrow->pos_y = main_widget_priv.screen_size_y - 3;
  rarrow->size_x = 3;
  rarrow->size_y = 5;

  w->priv = &main_widget_priv;
  w->draw = main_widget_draw;
  w->on_tick = main_on_tick;
  w->handle_event = main_widget_handle_event;
  w->pos_x = 0;
  w->pos_y = 0;
  w->size_x = 128;
  w->size_y = 64;
  return 0;
}
