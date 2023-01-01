#include "ui.h"
#include "font.h"
#include <string.h>
#include "ssd1306.h"
#include "tim.h"
#include "pushbuttons.h"

#define UI_ID_MAIN 0
#define UI_ID_TEST1 1
#define UI_ID_TEST2 2
#define UI_ID_COUNT 3

#define BUTTON_ID_LEFT  0
#define BUTTON_ID_MID   1
#define BUTTON_ID_RIGHT 2
typedef enum {
  UI_EVENT_BUTTON_PRESSED,
  UI_EVENT_BUTTON_RELEASED,
} ui_event_type;

typedef void (*draw_fn)(void);
typedef void (*handle_event_fn)(ui_event_type ev, int param);

struct ui_view {
  int id;
  draw_fn draw;
  handle_event_fn handle_event;
};

static void ui_draw_main(void);
static void ui_draw_test1(void);
static void ui_draw_test2(void);
static void ui_handle_event_main(ui_event_type event, int param);
static void ui_handle_event_test1(ui_event_type event, int param);
static void ui_handle_event_test2(ui_event_type event, int param);

static const struct ui_view ui_views[] = {
  {
    .id = UI_ID_MAIN,
    .draw = ui_draw_main,
    .handle_event = ui_handle_event_main
  },
  { 
    .id = UI_ID_TEST1,
    .draw = ui_draw_test1,
    .handle_event = ui_handle_event_test1
  },
  {
    .id = UI_ID_TEST2,
    .draw = ui_draw_test2,
    .handle_event = ui_handle_event_test1
  }
};

static int ui_current_id = UI_ID_MAIN;

struct bar_widget {
  int pos_x;
  int pos_y;
  int size_x;
  int size_y;
  int level;
};

void bar_widget_draw(const struct bar_widget *b)
{
  dbuf_draw_rect(
    b->pos_x, b->pos_y,
    b->pos_x + b->size_x,
    b->pos_y + b->size_y, 1);

  dbuf_draw_hatched_rect(
    b->pos_x + 2,
    b->pos_y,
    b->pos_x + b->size_x - 1,
    b->pos_y + b->level,
    1);

  dbuf_draw_line(
    b->pos_x + 2,
    b->pos_y + b->level,
    b->pos_x + b->size_x - 1,
    b->pos_y + b->level,
    1);
}

void draw_tim2_cntr(int x, int y)
{
  char counterbuf[32];
  int val = tim2_read_counter_value() >> 4;
  itoa(val, counterbuf, 10);
  x = dbuf_draw_text(x, y, "TIM2_TCNT:", &font_1);
  dbuf_draw_text(x, y, counterbuf, &font_2);
}

void draw_dynamic_bar(int level)
{
  struct bar_widget w = {
    .pos_x = 110,
    .pos_y = 0,
    .size_x = 10,
    .size_y = 63,
    .level = level
  };
  bar_widget_draw(&w);
}

void draw_voltmeter(int x, int y, int volt_int, int volt_frac)
{
  char b[32];
  char *p = b;
  itoa(volt_int, p, 10);
  p += strlen(p);
  *p++ = '.';
  itoa(volt_frac, p, 10);
  p += strlen(p);
  x = dbuf_draw_text(x, y, "V:", &font_3);
  dbuf_draw_text(x, y, b, &font_3);
}

int frame_counter = 0;
int duration;

void draw_blinker_icon(int x, int y, int sz, int num_frames, int interval)
{
  if (!duration && !(frame_counter % interval)) {
    duration = num_frames;
  }
  if (duration) {
    dbuf_draw_filled_rect(x, y, x + sz, y + sz, 1);
    duration--;
  }
}

#if 0
static void update_display_usb_stats()
{
  char buf[32];
  dbuf_draw_text(20, 4, "r:", &font_1);
  itoa(usbstats.num_resets, buf, 10);
  dbuf_draw_text(28, 4, buf, &font_1);
  itoa(usbstats.num_errs, buf, 10);
  dbuf_draw_text(34, 4, buf, &font_1);
  itoa(usbstats.num_transactions, buf, 10);
  dbuf_draw_text(40, 4, buf, &font_1);
}
#endif

void ui_draw_main(void)
{
  int y;
  float adc_normalized = 0.0f;// (float)last_adc / 4096;
  int level = adc_normalized * 64;
  int num_volt = adc_normalized * 3300;
  int first = num_volt / 1000;
  int next = num_volt - first * 1000;
  draw_dynamic_bar(level);
  draw_voltmeter(2, 35, first, next);

  y = 40;
  dbuf_draw_line(64, y, 64, y + 3, 1);
  y = 20;
  dbuf_draw_line(64, y, 64, y + 3, 1);
  y = 5;
  dbuf_draw_line(64, y, 64, y + 3, 1);
  draw_tim2_cntr(10, 50);
  draw_blinker_icon(89, 6, 5, 3, 13);
  // update_display_usb_stats();
}

void ui_draw_test1(void)
{
  draw_tim2_cntr(10, 50);
}

void ui_draw_test2(void)
{
  draw_tim2_cntr(50, 30);
}

void ui_update(void)
{
  frame_counter++;
  dbuf_clear();
  ui_views[ui_current_id].draw();
  dbuf_flush();
}

static inline void set_next_screen(void)
{
  ui_current_id++;
  if (ui_current_id == UI_ID_COUNT)
    ui_current_id = 0;
}

static inline void set_prev_screen(void)
{
  ui_current_id--;
  if (ui_current_id == -1)
    ui_current_id = UI_ID_COUNT - 1;
}

static void on_button_pressed_main(int button_id)
{
  switch(button_id)
  {
    case BUTTON_ID_LEFT:
      set_prev_screen();
      break;
    case BUTTON_ID_MID:
      break;
    case BUTTON_ID_RIGHT:
      set_next_screen();
      break;
    default:
      break;
  }
}

static void on_button_pressed_test1(int button_id)
{
  switch(button_id)
  {
    case BUTTON_ID_LEFT:
      set_prev_screen();
      break;
    case BUTTON_ID_MID:
      break;
    case BUTTON_ID_RIGHT:
      set_next_screen();
      break;
    default:
      break;
  }
}

static void on_button_released_test1(int button_id)
{
}

static void on_button_pressed_test2(int button_id)
{
  switch(button_id)
  {
    case BUTTON_ID_LEFT:
      set_prev_screen();
      break;
    case BUTTON_ID_MID:
      break;
    case BUTTON_ID_RIGHT:
      set_next_screen();
      break;
    default:
      break;
  }
}

static void on_button_released_test2(int button_id)
{
}

static void on_button_released_main(int param)
{
}

static void ui_handle_event_main(ui_event_type event, int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
      on_button_pressed_main(param);
      break;
    case UI_EVENT_BUTTON_RELEASED:
      on_button_released_main(param);
      break;
    default:
      break;
  }
}

static void ui_handle_event_test1(ui_event_type event, int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
      on_button_pressed_test1(param);
      break;
    case UI_EVENT_BUTTON_RELEASED:
      on_button_released_test1(param);
      break;
    default:
      break;
  }
}

static void ui_handle_event_test2(ui_event_type event, int param)
{
  switch (event)
  {
    case UI_EVENT_BUTTON_PRESSED:
      on_button_pressed_test2(param);
      break;
    case UI_EVENT_BUTTON_RELEASED:
      on_button_released_test2(param);
      break;
    default:
      break;
  }
}

void ui_callback_button_event_pressed(int button_id)
{
  ui_views[ui_current_id].handle_event(UI_EVENT_BUTTON_PRESSED, button_id);
}

void ui_callback_button_event_released(int button_id)
{
  ui_views[ui_current_id].handle_event(UI_EVENT_BUTTON_RELEASED, button_id);
}

